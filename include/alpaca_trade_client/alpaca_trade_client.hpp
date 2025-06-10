#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/system/error_code.hpp>
#include <boost/url.hpp>
#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <queue>
#include <string>
#include <utility>

namespace net   = boost::asio;
namespace ssl   = boost::asio::ssl;
namespace beast = boost::beast;
namespace http  = beast::http;
using tcp       = boost::asio::ip::tcp;

class alpaca_trade_client
  : public std::enable_shared_from_this<alpaca_trade_client>
{
public:

  //
  // Static methods

  static std::shared_ptr<alpaca_trade_client> create(
    net::io_context &ioc,
    ssl::context    &ctx);

  //
  // Configuration

  struct config
  {
    std::string          api_key;
    std::string          secret_key;
    std::string          endpoint { "https://paper-api.alpaca.markets" };
    std::chrono::seconds timeout { 30 };
    std::size_t          max_retries { 3 };
  };

  //
  // Overload tags

  struct qty_t
  {
    std::string value;

    explicit qty_t(std::string v)
      : value(std::move(v))
    {
    }
  };

  struct percentage_t
  {
    std::string value;

    explicit percentage_t(std::string v)
      : value(std::move(v))
    {
    }
  };

  //
  // Public methods

  void connect(const config &cfg);

  //
  // /v2/orders

  template <typename OrderType, typename CompletionHandler>
  void submit_order(const OrderType &order, CompletionHandler &&handler);

  //
  // /v2/positions

  template <typename CompletionHandler>
  void get_all_positions(CompletionHandler &&handler);

  template <typename CompletionHandler>
  void get_position(
    const std::string  &symbol_or_asset_id,
    CompletionHandler &&handler);

  template <typename CompletionHandler>
  void close_all_positions(bool cancel_orders, CompletionHandler &&handler);

  template <typename CompletionHandler>
  void close_position(
    const std::string  &symbol_or_asset_id,
    CompletionHandler &&handler);

  template <typename CompletionHandler>
  void close_position(
    const std::string  &symbol_or_asset_id,
    const qty_t        &qty,
    CompletionHandler &&handler);

  template <typename CompletionHandler>
  void close_position(
    const std::string  &symbol_or_asset_id,
    const percentage_t &percentage,
    CompletionHandler &&handler);

  bool is_connected() const;

  void disconnect();

private:

  using completion_handler_t
    = std::function<void(beast::error_code, const nlohmann::json &)>;
  using response_parser_t
    = std::function<nlohmann::json(const http::response<http::string_body> &)>;

  struct task
  {
    http::request<http::string_body> _request;
    completion_handler_t             _handler;
    response_parser_t                _response_parser;
  };

  struct request_info
  {
    http::verb  verb;
    boost::url  url;
    std::string body {};
  };

  //
  // Response parsers

  static nlohmann::json parse_response(
    const http::response<http::string_body> &response);

  static nlohmann::json parse_order_response(
    const http::response<http::string_body> &response);

  static nlohmann::json parse_positions_response(
    const http::response<http::string_body> &response);

  //
  // Send helpers

  template <typename CompletionHandler>
  void execute_async_request(
    const request_info &req_info,
    CompletionHandler &&handler,
    response_parser_t   response_parser = parse_response);

  void process_request_queue();
  void send_next_request();
  void on_write(beast::error_code ec, std::size_t bytes_transferred);
  void on_read(beast::error_code ec, std::size_t bytes_transferred);

  http::request<http::string_body> create_request(
    const request_info &req_info);

  void setup_request_headers(http::request<http::string_body> &req);

  //
  // Private ctor to prevent direct instantiation

  alpaca_trade_client(net::io_context &ioc, ssl::context &ctx);

  //
  // Boost.Asio components

  tcp::resolver                  _resolver;
  ssl::stream<beast::tcp_stream> _stream;
  beast::flat_buffer             _buffer {};

  //
  // Configuration

  std::string          _api_key {};
  std::string          _secret_key {};
  std::string          _host {};
  std::string          _port {};
  std::chrono::seconds _timeout { 30 };

  //
  // Connection state

  bool _connected { false };

  //
  // Request state
  std::queue<task>                  _request_queue {};
  bool                              _request_in_progress { false };
  http::response<http::string_body> _response {};
};

//
// Template implementations

template <typename CompletionHandler>
void
alpaca_trade_client::execute_async_request(
  const request_info &req_info,
  CompletionHandler &&handler,
  response_parser_t   response_parser)
{
  if (!_connected)
    {
      auto ec { boost::system::errc::make_error_code(
        boost::system::errc::not_connected) };
      handler(ec, nlohmann::json {});
      return;
    }

  try
    {
      auto request { create_request(req_info) };

      task async_task {};
      async_task._request         = std::move(request);
      async_task._handler         = std::forward<CompletionHandler>(handler);
      async_task._response_parser = std::move(response_parser);

      _request_queue.push(std::move(async_task));

      process_request_queue();
    }
  catch (const std::exception &e)
    {
      auto ec { boost::system::errc::make_error_code(
        boost::system::errc::invalid_argument) };
      handler(ec, nlohmann::json {});
    }
}

template <typename OrderType, typename CompletionHandler>
void
alpaca_trade_client::submit_order(
  const OrderType    &order,
  CompletionHandler &&handler)
{
  auto req_info {
    request_info { http::verb::post,
                  boost::url { "/v2/orders" },
                  order.to_json().dump() }
  };
  execute_async_request(
    req_info,
    std::forward<CompletionHandler>(handler),
    parse_order_response);
}

template <typename CompletionHandler>
void
alpaca_trade_client::get_all_positions(CompletionHandler &&handler)
{
  auto req_info {
    request_info { http::verb::get, boost::url { "/v2/positions" } }
  };
  execute_async_request(
    req_info,
    std::forward<CompletionHandler>(handler),
    parse_positions_response);
}

template <typename CompletionHandler>
void
alpaca_trade_client::get_position(
  const std::string  &symbol_or_asset_id,
  CompletionHandler &&handler)
{
  auto req_info {
    request_info { http::verb::get,
                  boost::url { "/v2/positions/" + symbol_or_asset_id } }
  };
  execute_async_request(
    req_info,
    std::forward<CompletionHandler>(handler),
    parse_response);
}

template <typename CompletionHandler>
void
alpaca_trade_client::close_all_positions(
  bool                cancel_orders,
  CompletionHandler &&handler)
{
  boost::url url { "/v2/positions" };
  if (cancel_orders)
    {
      url.params().append({ "cancel_orders", "true" });
    }
  auto req_info {
    request_info { http::verb::delete_, url }
  };

  execute_async_request(
    req_info,
    std::forward<CompletionHandler>(handler),
    parse_positions_response);
}

template <typename CompletionHandler>
void
alpaca_trade_client::close_position(
  const std::string  &symbol_or_asset_id,
  CompletionHandler &&handler)
{
  auto req_info {
    request_info { http::verb::delete_,
                  boost::url { "/v2/positions/" + symbol_or_asset_id } }
  };
  execute_async_request(
    req_info,
    std::forward<CompletionHandler>(handler),
    parse_order_response);
}

template <typename CompletionHandler>
void
alpaca_trade_client::close_position(
  const std::string  &symbol_or_asset_id,
  const qty_t        &qty,
  CompletionHandler &&handler)
{
  auto req_info {
    request_info { http::verb::delete_,
                  boost::url { "/v2/positions/" + symbol_or_asset_id } }
  };
  req_info.url.params().append({ "qty", qty.value });

  execute_async_request(
    req_info,
    std::forward<CompletionHandler>(handler),
    parse_order_response);
}

template <typename CompletionHandler>
void
alpaca_trade_client::close_position(
  const std::string  &symbol_or_asset_id,
  const percentage_t &percentage,
  CompletionHandler &&handler)
{
  auto req_info {
    request_info { http::verb::delete_,
                  boost::url { "/v2/positions/" + symbol_or_asset_id } }
  };
  req_info.url.params().append({ "percentage", percentage.value });

  execute_async_request(
    req_info,
    std::forward<CompletionHandler>(handler),
    parse_order_response);
}
