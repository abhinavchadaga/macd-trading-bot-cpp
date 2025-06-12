#include "async_rest_client/async_rest_client.hpp"

#include "LoggingUtils.hpp"
#include "async_rest_client/connection_context.hpp"
#include "async_rest_client/typed_task.hpp"
#include "async_rest_client/utils.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/url.hpp>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <openssl/ssl.h>
#include <stdexcept>
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
  , _tcp_stream { _ioc }
  , _ssl_stream { _ioc, _ssl_ctx }
{
  configure_logging();
  CLASS_LOGGER(async_rest_client);
  _ssl_ctx.set_default_verify_paths();
  _ssl_ctx.set_verify_mode(ssl::verify_peer);
}

async_rest_client::~async_rest_client()
{
  if (_is_processing || !_tasks.empty())
    std::abort();
}

net::awaitable<bool>
async_rest_client::connect(std::string_view url_sv)
{
  if (_connection_state == connection_state::CONNECTING)
    {
      LOG_WARNING(async_rest_client, connect)
        << "Calling connect while already connecting";
      co_return false;
    }

  _connection_state = connection_state::CONNECTING;
  boost::url url    = make_http_https_url(url_sv);
  _is_tls           = url.scheme() == "https";

  if (_is_tls)
    {
      if (!SSL_set_tlsext_host_name(
            _ssl_stream.native_handle(),
            url.host().c_str()))
        {
          boost::system::error_code ec;
          ec.assign(
            static_cast<int>(::ERR_get_error()),
            net::error::get_ssl_category());
          _connection_state = connection_state::NOT_CONNECTED;
          throw boost::system::system_error(ec, "SNI hostname");
        }
    }

  LOG_INFO(async_rest_client, connect) << "Starting resolve";
  tcp::resolver resolver { _ioc };
  auto [resolve_ec, endpoints] { co_await resolver.async_resolve(
    url.host(),
    url.port(),
    net::as_tuple(net::use_awaitable)) };

  LOG_INFO(async_rest_client, connect) << "Resolved endpoints";

  if (resolve_ec)
    {
      _connection_state = connection_state::NOT_CONNECTED;
      throw boost::system::system_error(resolve_ec, "resolve");
    }

  LOG_DEBUG(async_rest_client, connect) << "Resolved url: " << url_sv;
  if (_is_tls)
    {
      beast::get_lowest_layer(_ssl_stream)
        .expires_after(std::chrono::seconds(30));
      auto [connect_ec, endpoint]
        = co_await beast::get_lowest_layer(_ssl_stream)
            .async_connect(endpoints, net::as_tuple(net::use_awaitable));

      if (connect_ec)
        {
          _connection_state = connection_state::NOT_CONNECTED;
          throw boost::system::system_error(connect_ec, "connect");
        }

      beast::get_lowest_layer(_ssl_stream)
        .expires_after(std::chrono::seconds(30));
      auto [handshake_ec] = co_await _ssl_stream.async_handshake(
        ssl::stream_base::client,
        net::as_tuple(net::use_awaitable));

      if (handshake_ec)
        {
          _connection_state = connection_state::NOT_CONNECTED;
          throw boost::system::system_error(handshake_ec, "handshake");
        }
    }
  else
    {
      _tcp_stream.expires_after(std::chrono::seconds(30));
      auto [connect_ec, endpoint] = co_await _tcp_stream.async_connect(
        endpoints,
        net::as_tuple(net::use_awaitable));

      if (connect_ec)
        {
          _connection_state = connection_state::NOT_CONNECTED;
          throw boost::system::system_error(connect_ec, "connect");
        }
    }

  _connection_state = connection_state::CONNECTED;
  _current_origin   = std::move(url);
  LOG_INFO(async_rest_client, connect)
    << "Connected to " << url_sv << " successfully";
  co_return true;
}

void
async_rest_client::enqueue_task(std::unique_ptr<base_task> task)
{
  _tasks.push_back(std::move(task));

  if (!_is_processing)
    {
      LOG_INFO(async_rest_client, enqueue_task)
        << "Spawning process_queue() co-routine";
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
      const auto &task = _tasks.front();
      bool        success { false };

      try
        {
          const boost::url &task_endpoint = task->endpoint();

          bool is_new_connection_required = false;

          if (_connection_state != connection_state::CONNECTED)
            {
              is_new_connection_required = true;
              LOG_INFO(async_rest_client, process_queue)
                << "Connecting to " << task_endpoint.host();
            }
          else if (
            _current_origin.encoded_origin() != task_endpoint.encoded_origin())
            {
              is_new_connection_required = true;
              LOG_INFO(async_rest_client, process_queue)
                << "Host switch detected: " << _current_origin.host() << " -> "
                << task_endpoint.host() << ", reconnecting...";
              co_await graceful_shutdown();
            }

          if (is_new_connection_required)
            {
              std::string connection_url { task_endpoint.encoded_origin() };
              if (const bool connected = co_await connect(connection_url);
                  !connected)
                {
                  LOG_ERROR(async_rest_client, process_queue)
                    << "Failed to connect to " << connection_url;
                  _tasks.pop_front();
                  continue;
                }
            }

          // Execute the task
          if (_is_tls)
            {
              connection_context ctx { _ssl_stream, _buffer };
              success = co_await task->run(ctx);
            }
          else
            {
              connection_context ctx { _tcp_stream, _buffer };
              success = co_await task->run(ctx);
            }
        }
      catch (const std::exception &e)
        {
          LOG_ERROR(async_rest_client, process_queue)
            << "Task execution failed: " << e.what();
          success = false;
        }

      _tasks.pop_front();

      if (!success)
        {
          LOG_WARNING(async_rest_client, process_queue)
            << "Task failed, continuing with next task";
        }
    }

  _is_processing = false;
  co_return;
}

net::awaitable<void>
async_rest_client::graceful_shutdown()
{
  if (_connection_state != connection_state::CONNECTED)
    {
      co_return;
    }

  try
    {
      if (_is_tls)
        {
          auto [ec] = co_await _ssl_stream.async_shutdown(
            net::as_tuple(net::use_awaitable));
          if (ec == net::error::eof)
            {
              ec = {};
            }

          if (ec)
            {
              throw boost::system::system_error { ec, "SSL shutdown failed" };
            }
          beast::get_lowest_layer(_ssl_stream).close();
        }
      else
        {
          _tcp_stream.close();
        }
    }
  catch (const std::exception &e)
    {
      LOG_WARNING(async_rest_client, graceful_shutdown)
        << "Error during shutdown: " << e.what();
    }

  _buffer.consume(_buffer.size());

  _current_origin.clear();
  _is_tls = false;

  _connection_state = connection_state::NOT_CONNECTED;
  LOG_INFO(async_rest_client, graceful_shutdown) << "Connection closed";
  co_return;
}

template <typename ResponseBody>
  requires SupportedResponseBody<ResponseBody>
net::awaitable<http::response<ResponseBody>>
async_rest_client::get(const std::string &url, const http::fields &headers)
{
  auto task = std::make_unique<typed_task<http::empty_body, ResponseBody>>(
    url,
    http::verb::get,
    headers,
    http::empty_body::value_type {});

  auto future = task->get_shared_future();

  enqueue_task(std::move(task));

  LOG_INFO(async_rest_client, get) << "about to call future.get()";
  co_return future.get();
}

template <typename RequestBody, typename ResponseBody>
  requires SupportedRequestBody<RequestBody>
        && SupportedResponseBody<ResponseBody>
net::awaitable<http::response<ResponseBody>>
async_rest_client::post(
  const std::string               &url,
  const http::fields              &headers,
  typename RequestBody::value_type body)
{
  auto task = std::make_unique<typed_task<RequestBody, ResponseBody>>(
    url,
    http::verb::post,
    headers,
    std::move(body));

  auto future = task->get_shared_future();

  enqueue_task(std::move(task));

  co_return future.get();
}

// Explicit template instantiations for supported combinations
template net::awaitable<http::response<http::string_body>>
async_rest_client::get<http::string_body>(
  const std::string  &url,
  const http::fields &headers);

template net::awaitable<http::response<http::string_body>>
async_rest_client::post<http::string_body, http::string_body>(
  const std::string            &url,
  const http::fields           &headers,
  http::string_body::value_type body);

} // namespace async_rest_client
