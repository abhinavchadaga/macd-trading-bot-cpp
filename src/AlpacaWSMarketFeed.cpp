#include "AlpacaWSMarketFeed.hpp"

#include "Bar.hpp"
#include "LoggingUtils.hpp"
#include "Utils.hpp"

#include <sstream>

alpaca_ws_market_feed::alpaca_ws_market_feed(asio::io_context &ioc, config cfg)
  : _ioc { ioc }
  , _config { std::move(cfg) }
  , _ssl_context { ssl::context::tls_client }
{
  configure_logging();
  CLASS_LOGGER(alpaca_ws_market_feed);
  _ssl_context.set_verify_mode(ssl::context::verify_none);
}

void
alpaca_ws_market_feed::start()
{
  std::string host {};
  std::string port {};

  if (!_config.host.empty())
    {
      host = _config.host;
      port = _config.port.empty() ? "8765" : _config.port;
    }
  else if (_config.sandbox)
    {
      host = "stream.data.sandbox.alpaca.markets";
      port = "443";
    }
  else
    {
      host = "stream.data.alpaca.markets";
      port = "443";
    }

  std::string endpoint;
  if (_config.test_mode)
    {
      endpoint = "/v2/test";
    }
  else
    {
      endpoint = "/v2/" + _config.feed;
    }

  const web_socket_session_config ws_config { .host     = host,
                                           .port     = port,
                                           .endpoint = endpoint,
                                           .auth_msg = "",
                                           .sub_msg  = "",
                                           .ssl_ctxt = _ssl_context };

  _ws_session = web_socket_session::create(
    _ioc,
    ws_config,
    [this](const std::string_view frame) {
      on_websocket_frame(frame);
    });

  _ws_session->start();
}

void
alpaca_ws_market_feed::stop() const
{
  if (_ws_session)
    {
      _ws_session->stop();
    }
}

void
alpaca_ws_market_feed::subscribe_to_bars(const std::vector<std::string> &symbols)
{
  _subscribed_symbols = symbols;
  if (_authenticated && _ws_session)
    {
      send_subscription_message();
    }
}

void
alpaca_ws_market_feed::subscribe_to_all_bars()
{
  _subscribed_symbols = { "*" };
  if (_authenticated && _ws_session)
    {
      send_subscription_message();
    }
}

boost::signals2::connection
alpaca_ws_market_feed::connect_bar_handler(const bar_signal_t::slot_type &handler)
{
  return _bar_signal.connect(handler);
}

void
alpaca_ws_market_feed::on_websocket_frame(std::string_view frame)
{
  try
    {
      const auto json_array = nlohmann::json::parse(frame);

      if (!json_array.is_array() || json_array.empty())
        {
          LOG_ERROR(alpaca_ws_market_feed, on_websocket_frame)
            << "Expected non-empty JSON array, got: " << frame;
          return;
        }

      for (const auto &json_msg : json_array)
        {
          if (!json_msg.contains("T"))
            continue;

          if (const std::string msg_type = json_msg["T"];
              msg_type == "success" && json_msg.contains("msg"))
            {
              if (const std::string msg = json_msg["msg"]; msg == "connected")
                {
                  LOG_INFO(alpaca_ws_market_feed, on_websocket_frame)
                    << "Connected to Alpaca feed, sending auth...";
                  send_auth_message();
                }
              else if (msg == "authenticated")
                {
                  LOG_INFO(alpaca_ws_market_feed, on_websocket_frame)
                    << "Authenticated successfully";
                  _authenticated = true;
                  if (!_subscribed_symbols.empty())
                    {
                      send_subscription_message();
                    }
                }
            }
          else if (msg_type == "b")
            {
              parse_bar_message(json_msg);
            }
          else if (msg_type == "error")
            {
              LOG_ERROR(alpaca_ws_market_feed, on_websocket_frame)
                << "Alpaca feed error: " << json_msg.dump();
            }
        }
    }
  catch (const std::exception &e)
    {
      LOG_ERROR(alpaca_ws_market_feed, on_websocket_frame)
        << "Error parsing JSON frame: " << e.what();
      LOG_ERROR(alpaca_ws_market_feed, on_websocket_frame) << "Frame: " << frame;
    }
}

void
alpaca_ws_market_feed::send_auth_message()
{
  nlohmann::json auth_msg = {
    { "action",             "auth" },
    {    "key",    _config.api_key },
    { "secret", _config.api_secret }
  };

  if (_ws_session)
    {
      _ws_session->send(auth_msg.dump());
    }
}

void
alpaca_ws_market_feed::send_subscription_message()
{
  const nlohmann::json sub_msg = {
    { "action",         "subscribe" },
    {   "bars", _subscribed_symbols }
  };

  if (_ws_session)
    {
      _ws_session->send(sub_msg.dump());
      _subscribed = true;
      std::ostringstream symbols_stream;
      symbols_stream << "Subscribed to bars for symbols: ";
      for (const auto &symbol : _subscribed_symbols)
        {
          symbols_stream << symbol << " ";
        }
      LOG_INFO(alpaca_ws_market_feed, send_subscription_message)
        << symbols_stream.str();
    }
}

void
alpaca_ws_market_feed::parse_bar_message(const nlohmann::json &message)
{
  try
    {
      const std::string symbol        = message["S"];
      const double      open          = message["o"];
      const double      high          = message["h"];
      const double      low           = message["l"];
      const double      close         = message["c"];
      const uint64_t    volume        = message["v"];
      const std::string timestamp_str = message["t"];

      bar_1min::timestamp timestamp { parse_rfc3339_utc_timestamp(timestamp_str) };
      bar_1min new_bar { symbol, open, high, low, close, volume, timestamp };
      _bar_signal(new_bar);
    }
  catch (const std::exception &e)
    {
      LOG_ERROR(alpaca_ws_market_feed, parse_bar_message)
        << "Error parsing bar message: " << e.what();
      LOG_ERROR(alpaca_ws_market_feed, parse_bar_message)
        << "Message: " << message.dump();
    }
}

std::string
alpaca_ws_market_feed::get_websocket_url() const
{
  if (_config.test_mode)
    return "wss://stream.data.alpaca.markets/v2/test";
  if (_config.sandbox)
    return "wss://stream.data.sandbox.alpaca.markets/v2/" + _config.feed;
  return "wss://stream.data.alpaca.markets/v2/" + _config.feed;
}
