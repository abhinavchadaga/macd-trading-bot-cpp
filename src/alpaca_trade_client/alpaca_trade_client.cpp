#include "alpaca_trade_client/alpaca_trade_client.hpp"

#include "LoggingUtils.hpp"

#include <boost/url.hpp>

//
// Public static methods

std::shared_ptr<alpaca_trade_client>
alpaca_trade_client::create(net::io_context &ioc, ssl::context &ctx)
{
  return std::shared_ptr<alpaca_trade_client>(
    new alpaca_trade_client(ioc, ctx));
}

alpaca_trade_client::alpaca_trade_client(
  net::io_context &ioc,
  ssl::context    &ctx)
  : _resolver(ioc)
  , _stream(ioc, ctx)
{
  CLASS_LOGGER(alpaca_trade_client);
  configure_logging();
}

//
// Public methods

void
alpaca_trade_client::connect(const config &cfg)
{
  try
    {
      _api_key    = cfg.api_key;
      _secret_key = cfg.secret_key;
      _endpoint   = cfg.endpoint;
      _timeout    = cfg.timeout;

      boost::url  url { cfg.endpoint };
      std::string host { url.host() };
      std::string port { url.port().empty() ? "443"
                                            : std::string { url.port() } };

      if (!SSL_set_tlsext_host_name(_stream.native_handle(), host.c_str()))
        {
          beast::error_code ec { static_cast<int>(::ERR_get_error()),
                                 net::error::get_ssl_category() };
          throw std::runtime_error(
            "Failed to set SNI hostname: " + ec.message());
        }

      _stream.set_verify_mode(ssl::verify_peer);
      _stream.set_verify_callback(ssl::host_name_verification(host));

      LOG_DEBUG(alpaca_trade_client, connect)
        << "Resolving Alpaca API host: " << host;
      auto const results { _resolver.resolve(host, port) };

      LOG_DEBUG(alpaca_trade_client, connect) << "Connecting to Alpaca API";
      beast::get_lowest_layer(_stream).expires_after(_timeout);
      auto ep { beast::get_lowest_layer(_stream).connect(results) };

      LOG_DEBUG(alpaca_trade_client, connect) << "Performing SSL handshake";
      _stream.handshake(ssl::stream_base::client);

      _connected = true;
      LOG_DEBUG(alpaca_trade_client, connect)
        << "Successfully connected to Alpaca API at "
        << ep.address().to_string() << ":" << ep.port();
    }
  catch (const std::exception &e)
    {
      _connected = false;
      LOG_ERROR(alpaca_trade_client, connect)
        << "Failed to connect to Alpaca API: " << e.what();
      throw;
    }
}

bool
alpaca_trade_client::is_connected() const
{
  return _connected;
}

void
alpaca_trade_client::disconnect()
{
  if (!_connected)
    {
      LOG_WARNING(alpaca_trade_client, disconnect)
        << "calling disconnect() when not connected";
      return;
    }

  try
    {
      LOG_DEBUG(alpaca_trade_client, disconnect)
        << "Disconnecting from Alpaca API...";
      beast::get_lowest_layer(_stream).expires_after(_timeout);

      beast::error_code ec { _stream.shutdown(ec) };

      if (ec && ec != net::ssl::error::stream_truncated)
        {
          LOG_ERROR(alpaca_trade_client, disconnect)
            << "Error during disconnect: " << ec.message();
        }

      _connected = false;
      LOG_DEBUG(alpaca_trade_client, disconnect)
        << "Successfully disconnected from Alpaca API";
    }
  catch (const std::exception &e)
    {
      LOG_ERROR(alpaca_trade_client, disconnect)
        << "Error during disconnect: " << e.what();
      _connected = false;
    }
}

//
// Private static methods

nlohmann::json
alpaca_trade_client::parse_response(
  const http::response<http::string_body> &response)
{
  auto status { response.result_int() };

  if (status == 201)
    {
      try
        {
          return nlohmann::json::parse(response.body());
        }
      catch (const nlohmann::json::parse_error &e)
        {
          std::cerr << "ERROR [alpaca_trade_client::parse_response] Failed to "
                       "parse JSON response: "
                    << e.what() << std::endl;
          throw std::runtime_error(
            "Invalid JSON in API response: " + std::string(e.what()));
        }
    }

  std::string error_msg { "HTTP " + std::to_string(status) };

  if (status == 400)
    {
      error_msg += " - Bad Request: Invalid order parameters";
    }
  else if (status == 401)
    {
      error_msg += " - Unauthorized: Invalid API credentials";
    }
  else if (status == 403)
    {
      error_msg += " - Forbidden: Access denied";
    }
  else if (status == 422)
    {
      error_msg += " - Unprocessable Entity: Order validation failed";
    }
  else if (status == 429)
    {
      error_msg += " - Rate limit exceeded";
    }
  else if (status >= 500)
    {
      error_msg += " - Server error";
    }
  else
    {
      error_msg += " - Unexpected response";
    }

  if (!response.body().empty())
    {
      try
        {
          auto error_json { nlohmann::json::parse(response.body()) };
          if (error_json.contains("message"))
            {
              error_msg += ": " + error_json["message"].get<std::string>();
            }
        }
      catch (const nlohmann::json::parse_error &)
        {
          error_msg += ": " + response.body();
        }
    }

  std::cerr << "ERROR [alpaca_trade_client::parse_response] " << error_msg
            << std::endl;
  throw std::runtime_error(error_msg);
}

//
// Private methods

void
alpaca_trade_client::setup_request_headers(
  http::request<http::string_body> &req)
{
  req.set(http::field::content_type, "application/json");
  req.set(http::field::user_agent, "macd-trading-bot/1.0");
  req.set("APCA-API-KEY-ID", _api_key);
  req.set("APCA-API-SECRET-KEY", _secret_key);
}

template <typename OrderType>
http::request<http::string_body>
alpaca_trade_client::create_order_request(const OrderType &order)
{
  http::request<http::string_body> req { http::verb::post, "/v2/orders", 11 };

  boost::url  url { _endpoint };
  std::string host { url.host() };
  req.set(http::field::host, host);

  req.body() = order.to_json().dump();
  req.prepare_payload();

  setup_request_headers(req);

  return req;
}
