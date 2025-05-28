#pragma once

#include "WebSocketSession.hpp"
#include "types.hpp"
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace asio = boost::asio;
namespace ssl = asio::ssl;

class AlpacaWSMarketFeed
{
public:
  using bar_signal_t = boost::signals2::signal<void (const Bar &)>;

  struct config
  {
    std::string api_key{};
    std::string api_secret{};
    std::string feed{ "iex" };
    bool sandbox{ false };
    bool test_mode{ true };
  };

  explicit AlpacaWSMarketFeed (asio::io_context &ioc, config cfg);

  void start ();

  void stop () const;

  void subscribe_to_bars (const std::vector<std::string> &symbols);

  void subscribe_to_all_bars ();

  boost::signals2::connection
  connect_bar_handler (const bar_signal_t::slot_type &handler);

  [[nodiscard]] std::string get_websocket_url () const;

private:
  void on_websocket_frame (std::string_view frame);

  void send_auth_message ();

  void send_subscription_message ();

  void parse_bar_message (const nlohmann::json &message);

  asio::io_context &_ioc;
  config _config;
  ssl::context _ssl_context;
  std::shared_ptr<WebSocketSession> _ws_session;

  bar_signal_t _bar_signal;
  std::vector<std::string> _subscribed_symbols;
  bool _authenticated{ false };
  bool _subscribed{ false };
};
