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
      _timeout    = cfg.timeout;

      boost::url url { cfg.endpoint };
      _host = std::string { url.host() };
      _port = url.port().empty() ? "443" : std::string { url.port() };

      if (!SSL_set_tlsext_host_name(_stream.native_handle(), _host.c_str()))
        {
          beast::error_code ec { static_cast<int>(::ERR_get_error()),
                                 net::error::get_ssl_category() };
          throw std::runtime_error(
            "Failed to set SNI hostname: " + ec.message());
        }

      _stream.set_verify_mode(ssl::verify_peer);
      _stream.set_verify_callback(ssl::host_name_verification(_host));

      LOG_DEBUG(alpaca_trade_client, connect)
        << "Resolving Alpaca API host: " << _host;
      auto const results { _resolver.resolve(_host, _port) };

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
  if (response.result() == http::status::ok)
    {
      try
        {
          return nlohmann::json::parse(response.body());
        }
      catch (const nlohmann::json::parse_error &e)
        {
          LOG_ERROR(alpaca_trade_client, parse_response)
            << "Failed to parse JSON response: " << e.what();
          throw std::runtime_error(
            "Invalid JSON in API response: " + std::string(e.what()));
        }
    }

  std::string error_msg { std::format("HTTP {}", response.result_int()) };

  if (response.result() == http::status::forbidden)
    {
      error_msg += " - Forbidden: Insufficient buying power";
    }
  else if (response.result() == http::status::unprocessable_entity)
    {
      error_msg += " - Unprocessable Entity: Input parameters not recognized";
    }
  else
    {
      LOG_WARNING(alpaca_trade_client, parse_response)
        << "Unrecognized error code from Alpaca API: " << response.result();
      error_msg += " - Unrecognized error code";
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

  LOG_ERROR(alpaca_trade_client, parse_response) << error_msg;
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

void
alpaca_trade_client::process_request_queue()
{
  if (_request_in_progress || _request_queue.empty())
    {
      return;
    }

  LOG_DEBUG(alpaca_trade_client, process_request_queue)
    << "Processing request queue, " << _request_queue.size()
    << " requests pending";

  send_next_request();
}

void
alpaca_trade_client::send_next_request()
{
  _request_in_progress = true;

  auto &pending_req { _request_queue.front() };

  LOG_DEBUG(alpaca_trade_client, send_next_request)
    << "Sending request to " << pending_req._request.target();

  http::async_write(
    _stream,
    pending_req._request,
    [self = shared_from_this()](
      beast::error_code ec,
      std::size_t bytes_transferred) {
      self->on_write(ec, bytes_transferred);
    });
}

void
alpaca_trade_client::on_write(
  beast::error_code ec,
  std::size_t       bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);

  if (ec)
    {
      LOG_ERROR(alpaca_trade_client, on_write)
        << "Failed to write request: " << ec.message();

      auto &pending_req { _request_queue.front() };
      pending_req._handler(ec, nlohmann::json {});
      _request_queue.pop();
      _request_in_progress = false;

      process_request_queue();
      return;
    }

  LOG_DEBUG(alpaca_trade_client, on_write)
    << "Successfully wrote " << bytes_transferred << " bytes";

  http::async_read(
    _stream,
    _buffer,
    _response,
    [self = shared_from_this()](
      beast::error_code ec,
      std::size_t bytes_transferred) {
      self->on_read(ec, bytes_transferred);
    });
}

void
alpaca_trade_client::on_read(
  beast::error_code ec,
  std::size_t       bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);

  auto &pending_req { _request_queue.front() };

  if (ec)
    {
      LOG_ERROR(alpaca_trade_client, on_read)
        << "Failed to read response: " << ec.message();

      pending_req._handler(ec, nlohmann::json {});
    }
  else
    {
      LOG_DEBUG(alpaca_trade_client, on_read)
        << "Successfully read " << bytes_transferred << " bytes";

      try
        {
          auto json_response { parse_response(_response) };
          pending_req._handler(ec, json_response);
        }
      catch (const std::exception &e)
        {
          LOG_ERROR(alpaca_trade_client, on_read)
            << "Failed to parse response: " << e.what();

          auto parse_error { boost::system::errc::make_error_code(
            boost::system::errc::invalid_argument) };
          pending_req._handler(parse_error, nlohmann::json {});
        }
    }

  _request_queue.pop();
  _request_in_progress = false;
  _response.clear();
  _buffer.clear();

  process_request_queue();
}
