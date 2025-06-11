#pragma once

#include "base_task.hpp"
#include "concepts.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/url.hpp>
#include <deque>
#include <memory>

namespace async_rest_client
{

namespace beast     = boost::beast;
namespace http      = beast::http;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
namespace ssl       = boost::asio::ssl;
using tcp           = boost::asio::ip::tcp;

enum class connection_state
{
  NOT_CONNECTED,
  CONNECTING,
  CONNECTED
};

class async_rest_client
  : public std::enable_shared_from_this<async_rest_client>
{
public:

  static std::shared_ptr<async_rest_client> create(net::io_context &ioc);

  net::awaitable<bool> connect(std::string_view url);

  template <typename ResponseBody>
    requires SupportedResponseBody<ResponseBody>
  net::awaitable<http::response<ResponseBody>> get(
    const std::string  &url,
    const http::fields &headers = {});

  template <typename RequestBody, typename ResponseBody>
    requires SupportedRequestBody<RequestBody>
          && SupportedResponseBody<ResponseBody>
  net::awaitable<http::response<ResponseBody>> post(
    const std::string               &url,
    const http::fields              &headers,
    typename RequestBody::value_type body);

private:

  explicit async_rest_client(net::io_context &ioc);

  std::deque<std::unique_ptr<base_task>> _tasks;
  bool                                   _processing { false };

  net::io_context                     &_ioc;
  ssl::context                         _ssl_ctx;
  beast::tcp_stream                    _tcp_stream;
  beast::ssl_stream<beast::tcp_stream> _ssl_stream;
  beast::flat_buffer                   _buffer {};

  boost::url       _current_host {};
  connection_state _connection_state { connection_state::NOT_CONNECTED };
  bool             _is_tls { false };
};

} // namespace async_rest_client
