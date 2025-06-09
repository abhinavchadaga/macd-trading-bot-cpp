#pragma once

#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = boost::asio::ssl;
using tcp       = boost::asio::ip::tcp;

class async_rest_client
  : public std::enable_shared_from_this<async_rest_client>
{
public:

  //
  // Static functions

  static std::shared_ptr<async_rest_client>
  create(const std::string &name, net::io_context &ioc, ssl::context &ctxt);


private:

  //
  // Private ctor to force use of factory function

  async_rest_client(
    std::string      name,
    net::io_context &ioc,
    ssl::context    &ctxt);


private:

  //
  // user agent
  std::string _name {};

  //
  // Boost Beast state
  tcp::resolver                     _resolver;
  ssl::stream<beast::tcp_stream>    _stream;
  beast::flat_buffer                _buffer;
  http::request<http::empty_body>   _req;
  http::response<http::string_body> _res;
};
