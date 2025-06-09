#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <queue>
#include <string>

namespace net   = boost::asio;
namespace ssl   = boost::asio::ssl;
namespace beast = boost::beast;
namespace http  = beast::http;
using tcp       = boost::asio::ip::tcp;

class alpaca_trade_client
  : public std::enable_shared_from_this<alpaca_trade_client>
{
public:

  struct config
  {
    std::string          api_key;
    std::string          secret_key;
    std::string          endpoint { "https://paper-api.alpaca.markets" };
    std::chrono::seconds timeout { 30 };
    std::size_t          max_retries { 3 };
  };

  static std::shared_ptr<alpaca_trade_client> create(
    net::io_context &ioc,
    ssl::context    &ctx);

  void connect(const config &cfg);

  template <typename OrderType, typename CompletionHandler>
  void submit_order(const OrderType &order, CompletionHandler &&handler);

  bool is_connected() const;

  void disconnect();

private:

  alpaca_trade_client(net::io_context &ioc, ssl::context &ctx);

  tcp::resolver                  _resolver;
  ssl::stream<beast::tcp_stream> _stream;
  beast::flat_buffer             _buffer;

  std::string          _api_key {};
  std::string          _secret_key {};
  std::string          _endpoint {};
  std::chrono::seconds _timeout { 30 };

  bool _connected { false };

  struct pending_request
  {
    http::request<http::string_body>                               _request;
    std::function<void(beast::error_code, const nlohmann::json &)> _handler;
  };

  std::queue<pending_request> _request_queue;
  bool                        _request_in_progress { false };

  void process_request_queue();
  void send_next_request();
  void on_write(beast::error_code ec, std::size_t bytes_transferred);
  void on_read(beast::error_code ec, std::size_t bytes_transferred);

  template <typename OrderType>
  http::request<http::string_body> create_order_request(
    const OrderType &order);

  void           setup_request_headers(http::request<http::string_body> &req);
  nlohmann::json parse_response(
    const http::response<http::string_body> &response);

  void fail(beast::error_code ec, const char *what);
};
