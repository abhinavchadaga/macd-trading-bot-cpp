//
// Created by Abhinav  Chadaga on 6/19/25.
//

#include "alpaca_trade_client/alpaca_trade_client.hpp"
#include "nlohmann/json.hpp"

//
// alpaca_api_error class

alpaca_api_error::alpaca_api_error(const error_type type, const int http_status, std::string message)
    : _type{type},
      _http_status{http_status},
      _message{std::move(message)}
{
}

alpaca_api_error::error_type alpaca_api_error::type() const
{
    return _type;
}

int alpaca_api_error::http_status() const
{
    return _http_status;
}

const std::string& alpaca_api_error::message() const
{
    return _message;
}

//
// config class

alpaca_trade_client::config::config(std::string api_key, std::string api_secret, const bool paper_trading)
    : _api_key{std::move(api_key)},
      _api_secret{std::move(api_secret)},
      _base_url{paper_trading ? "https://paper-api.alpaca.markets/v2" : "https://api.alpaca.markets"}
{
}

//
// config accessors

std::string alpaca_trade_client::config::api_key() const
{
    return _api_key;
}
std::string alpaca_trade_client::config::api_secret() const
{
    return _api_secret;
}
std::string alpaca_trade_client::config::base_url() const
{
    return _base_url;
}

//
// alpaca_trade_client public static methods

std::shared_ptr<alpaca_trade_client> alpaca_trade_client::create(net::io_context& ioc, config config)
{
    return std::shared_ptr<alpaca_trade_client>(new alpaca_trade_client{ioc, std::move(config)});
}
net::awaitable<std::expected<std::string, alpaca_api_error>> alpaca_trade_client::effective_buying_power() const
{
    auto [ec, res]{
        co_await _rest_client->request<http::verb::get>(_cfg.base_url() + "/account", create_auth_headers())};

    if (ec)
    {
        co_return std::unexpected{alpaca_api_error{alpaca_api_error::error_type::network_error, 0, ec.message()}};
    }

    if (res.result() != http::status::ok)
    {
        co_return std::unexpected{
            alpaca_api_error{alpaca_api_error::error_type::http_error, static_cast<int>(res.result()), res.body()}};
    }

    try
    {
        const auto        json_body{nlohmann::json::parse(res.body())};
        const std::string effective_buying_power{json_body["effective_buying_power"]};
        co_return effective_buying_power;
    }
    catch (const nlohmann::json::exception& e)
    {
        co_return std::unexpected{
            alpaca_api_error{alpaca_api_error::error_type::json_parse_error, static_cast<int>(res.result()), e.what()}};
    }
}

//
// alpaca_trade_client private methods

alpaca_trade_client::alpaca_trade_client(net::io_context& ioc, config cfg)
    : _cfg{std::move(cfg)},
      _rest_client{async_rest_client::async_rest_client::create(ioc)}
{
}

http::fields alpaca_trade_client::create_auth_headers() const
{
    http::fields headers;
    headers.set("APCA-API-KEY-ID", _cfg.api_key());
    headers.set("APCA-API-SECRET-KEY", _cfg.api_secret());
    return headers;
}
