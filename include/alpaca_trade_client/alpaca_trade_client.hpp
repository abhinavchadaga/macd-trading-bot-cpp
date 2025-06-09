#pragma once

#include "alpaca_trade_client/orders.hpp"

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

  /**
   * @brief Configuration for the alpaca_trade_client
   */
  struct config
  {
    std::string          api_key;
    std::string          secret_key;
    std::string          endpoint { "https://paper-api.alpaca.markets" };
    std::chrono::seconds timeout { 30 };
    std::size_t          max_retries { 3 };
  };

  /**
   * @brief Create a new alpaca_trade_client instance
   *
   * @param ioc I/O context
   * @param ctx SSL context
   * @return std::shared_ptr<alpaca_trade_client>
   */
  static std::shared_ptr<alpaca_trade_client> create(
    net::io_context &ioc,
    ssl::context    &ctx);

  /**
   * @brief Connect to the Alpaca API using the provided configuration
   * Occurs asynchronously.
   *
   * @param cfg Configuration
   */
  void connect(const config &cfg);

  /**
   * @brief Submit an order to the Alpaca API
   * Occurs asynchronously.
   *
   * @param order Order
   * @param handler Completion handler
   */
  template <typename OrderType, typename CompletionHandler>
  void submit_order(const OrderType &order, CompletionHandler &&handler);

  /**
   * @brief Check if the client is connected to the Alpaca API
   *
   * @return true if connected, false otherwise
   */
  bool is_connected() const;

  /**
   * @brief Disconnect from the Alpaca API
   * Occurs synchronously.
   */
  void disconnect();

private:

  struct pending_request
  {
    http::request<http::string_body>                               _request;
    std::function<void(beast::error_code, const nlohmann::json &)> _handler;
  };

  static nlohmann::json parse_response(
    const http::response<http::string_body> &response);

  void process_request_queue();
  void send_next_request();
  void on_write(beast::error_code ec, std::size_t bytes_transferred);
  void on_read(beast::error_code ec, std::size_t bytes_transferred);

  template <typename OrderType>
  http::request<http::string_body> create_order_request(
    const OrderType &order);

  void setup_request_headers(http::request<http::string_body> &req);

  //
  // Private ctor to prevent direct instantiation

  alpaca_trade_client(net::io_context &ioc, ssl::context &ctx);

  //
  // Boost.Asio components
  tcp::resolver                  _resolver;
  ssl::stream<beast::tcp_stream> _stream;
  beast::flat_buffer             _buffer;

  //
  // Configuration
  std::string          _api_key {};
  std::string          _secret_key {};
  std::string          _endpoint {};
  std::chrono::seconds _timeout { 30 };

  //
  // Connection state
  bool _connected { false };

  //
  // Request state
  std::queue<pending_request> _request_queue;
  bool                        _request_in_progress { false };
};
