#include "async_rest_client/async_rest_client.hpp"

#include "LoggingUtils.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/url.hpp>
#include <chrono>
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

net::awaitable<bool>
async_rest_client::connect(std::string_view url)
{
  if (_connection_state == connection_state::CONNECTING)
    {
      LOG_WARNING(async_rest_client, connect)
        << "Calling connect while already connecting";
      co_return false;
    }

  _connection_state = connection_state::CONNECTING;
  boost::url parsed_url { url };

  if (!parsed_url.has_scheme())
    {
      _connection_state = connection_state::NOT_CONNECTED;
      throw std::invalid_argument(
        "URL must include scheme (http:// or https://)");
    }

  std::string_view scheme { parsed_url.scheme() };
  if (scheme != "http" && scheme != "https")
    {
      _connection_state = connection_state::NOT_CONNECTED;
      throw std::invalid_argument("Only http and https schemes are supported");
    }

  std::string host { parsed_url.host() };
  std::string port { parsed_url.has_port()
                       ? std::string { parsed_url.port() }
                       : (scheme == "https" ? "443" : "80") };

  _is_tls       = (scheme == "https");
  _current_host = parsed_url;

  tcp::resolver             resolver { _ioc };
  boost::system::error_code ec;
  auto [resolve_ec, endpoints] { co_await resolver.async_resolve(
    host,
    port,
    net::as_tuple(net::use_awaitable)) };

  if (resolve_ec)
    {
      _connection_state = connection_state::NOT_CONNECTED;
      throw boost::system::system_error(resolve_ec, "resolve");
    }

  if (_is_tls)
    {
      if (!SSL_set_tlsext_host_name(_ssl_stream.native_handle(), host.c_str()))
        {
          ec.assign(
            static_cast<int>(::ERR_get_error()),
            net::error::get_ssl_category());
          _connection_state = connection_state::NOT_CONNECTED;
          throw boost::system::system_error(ec, "SNI hostname");
        }

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
  LOG_INFO(async_rest_client, connect)
    << "Connected to " << url << " successfully";
  co_return true;
}

} // namespace async_rest_client
