#pragma once

#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <memory>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = boost::asio::ssl;
using tcp       = boost::asio::ip::tcp;

class alpaca_trade_client : std::enable_shared_from_this<alpaca_trade_client>
{
  static const char *ENDPOINT;

public:

  static std::shared_ptr<alpaca_trade_client> create(
    net::io_context &ioc,
    ssl::context    &ctxt);

private:

  //
  // Private Ctor to force use of factory fn
  alpaca_trade_client(net::io_context &ioc, ssl::context &ctxt);


  tcp::resolver                     _resolver;
  ssl::stream<beast::tcp_stream>    _stream;
  beast::flat_buffer                _buffer;
  http::request<http::empty_body>   _req;
  http::response<http::string_body> _res;
};
