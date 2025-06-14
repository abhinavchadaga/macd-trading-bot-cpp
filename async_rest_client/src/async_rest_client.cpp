#include "async_rest_client/async_rest_client.hpp"

#include "async_rest_client/connection_context.hpp"
#include "async_rest_client/typed_task.hpp"
#include "async_rest_client/utils.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/url.hpp>
#include <cassert>
#include <chrono>
#include <memory>
#include <openssl/ssl.h>
#include <string_view>

namespace async_rest_client
{

std::shared_ptr<async_rest_client>
async_rest_client::create(net::io_context &ioc)
{
  return std::shared_ptr<async_rest_client> { new async_rest_client { ioc } };
}

async_rest_client::async_rest_client(net::io_context &ioc)
  : _ioc { ioc }
  , _ssl_ctx { ssl::context::tlsv12_client }
  , _tcp_stream { std::make_unique<beast::tcp_stream>(_ioc) }
  , _ssl_stream { std::make_unique<beast::ssl_stream<beast::tcp_stream>>(
      _ioc,
      _ssl_ctx) }
  , _buffer { std::make_unique<beast::flat_buffer>() }
{
  _ssl_ctx.set_default_verify_paths();
  _ssl_ctx.set_verify_mode(ssl::verify_peer);
}

async_rest_client::~async_rest_client()
{
  const boost::system::error_code ec { boost::system::errc::operation_canceled,
                                       boost::system::generic_category() };

  while (!_tasks.empty())
    {
      _tasks.front()->fail(ec);
      _tasks.pop_front();
    }

  if (_connection_state == connection_state::CONNECTED)
    {
      if (_is_tls)
        {
          boost::system::error_code shutdown_ec;
          _ssl_stream->shutdown(shutdown_ec); // NOLINT
          beast::get_lowest_layer(*_ssl_stream).close();
        }
      else
        {
          _tcp_stream->close();
        }
    }
}

net::awaitable<boost::system::error_code>
async_rest_client::connect(std::string_view url_sv)
{
  if (_connection_state == connection_state::CONNECTING)
    co_return boost::system::error_code {
      boost::system::errc::operation_in_progress,
      boost::system::generic_category()
    };

  _connection_state = connection_state::CONNECTING;

  boost::url url;
  try
    {
      url = make_http_https_url(url_sv);
    }
  catch (const boost::system::system_error &e)
    {
      _connection_state = connection_state::NOT_CONNECTED;
      co_return e.code();
    }

  _is_tls = url.scheme() == "https";

  if (
    _is_tls
    && !SSL_set_tlsext_host_name(
      _ssl_stream->native_handle(),
      url.host().c_str()))
    {
      boost::system::error_code ec { static_cast<int>(ERR_get_error()),
                                     net::error::get_ssl_category() };
      _connection_state = connection_state::NOT_CONNECTED;
      co_return ec;
    }

  tcp::resolver resolver { _ioc };
  auto [resolve_ec, endpoints] { co_await resolver.async_resolve(
    url.host(),
    url.port(),
    net::as_tuple(net::use_awaitable)) };


  if (resolve_ec)
    {
      _connection_state = connection_state::NOT_CONNECTED;
      co_return resolve_ec;
    }

  if (_is_tls)
    {
      beast::get_lowest_layer(*_ssl_stream)
        .expires_after(std::chrono::seconds(30));
      auto [connect_ec, endpoint]
        = co_await beast::get_lowest_layer(*_ssl_stream)
            .async_connect(endpoints, net::as_tuple(net::use_awaitable));

      if (connect_ec)
        {
          _connection_state = connection_state::NOT_CONNECTED;
          co_return connect_ec;
        }

      beast::get_lowest_layer(*_ssl_stream)
        .expires_after(std::chrono::seconds(30));
      auto [handshake_ec] = co_await _ssl_stream->async_handshake(
        ssl::stream_base::client,
        net::as_tuple(net::use_awaitable));

      if (handshake_ec)
        {
          _connection_state = connection_state::NOT_CONNECTED;
          co_return handshake_ec;
        }
    }
  else
    {
      _tcp_stream->expires_after(std::chrono::seconds(30));
      auto [connect_ec, endpoint] = co_await _tcp_stream->async_connect(
        endpoints,
        net::as_tuple(net::use_awaitable));

      if (connect_ec)
        {
          _connection_state = connection_state::NOT_CONNECTED;
          co_return connect_ec;
        }
    }

  _connection_state = connection_state::CONNECTED;
  _current_origin   = std::move(url);
  co_return make_error_code(boost::system::errc::success);
}

void
async_rest_client::enqueue_task(std::unique_ptr<base_task> &&task)
{
  _tasks.push_back(std::move(task));

  if (!_is_processing)
    {
      net::co_spawn(_ioc, process_queue(), net::detached);
    }
}

net::awaitable<void>
async_rest_client::process_queue()
{
  assert(
    !_is_processing && "process_queue is being launched more than one time");

  _is_processing = true;

  while (!_tasks.empty())
    {
      const std::unique_ptr<base_task> &current_task { _tasks.front() };

      const boost::url &task_endpoint { current_task->endpoint() };

      bool is_new_connection_required { false };

      if (_connection_state != connection_state::CONNECTED)
        {
          // Not current connect

          is_new_connection_required = true;
        }
      else if (
        _current_origin.encoded_origin() != task_endpoint.encoded_origin())
        {
          // Connected to a different host
          is_new_connection_required = true;
          co_await graceful_shutdown();
        }

      if (is_new_connection_required)
        {
          const std::string_view connection_url {
            task_endpoint.encoded_origin()
          };
          if (const auto connect_ec = co_await connect(connection_url))
            {
              // TODO: log connection error here
              current_task->fail(connect_ec);
              _tasks.pop_front();
              continue;
            }
        }

      if (_is_tls)
        {
          connection_context ctx { *_ssl_stream, *_buffer };
          co_await current_task->send(ctx);
        }
      else
        {
          connection_context ctx { *_tcp_stream, *_buffer };
          co_await current_task->send(ctx);
        }

      _tasks.pop_front();
    }

  _is_processing = false;

  co_return;
}

net::awaitable<void>
async_rest_client::graceful_shutdown()
{
  if (_connection_state != connection_state::CONNECTED)
    {
      // TODO: Log warning here
      co_return;
    }

  if (_is_tls)
    {
      auto [ec] = co_await _ssl_stream->async_shutdown(
        net::as_tuple(net::use_awaitable));

      if (ec == ssl::error::stream_truncated)
        ec = {};

      beast::get_lowest_layer(*_ssl_stream).close();
      _ssl_stream = std::make_unique<beast::ssl_stream<beast::tcp_stream>>(
        _ioc,
        _ssl_ctx);

      if (ec)
        {
          throw boost::system::system_error { ec, "SSL shutdown failed" };
        }
    }
  else
    {
      _tcp_stream->close();
      _tcp_stream = std::make_unique<beast::tcp_stream>(_ioc);
    }

  _buffer->consume(_buffer->size());

  _current_origin.clear();
  _is_tls = false;

  _connection_state = connection_state::NOT_CONNECTED;
  co_return;
}

} // namespace async_rest_client
