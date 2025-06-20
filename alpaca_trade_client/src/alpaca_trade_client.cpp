#include "alpaca_trade_client/alpaca_trade_client.hpp"
#include "nlohmann/json.hpp"
#include <iostream>

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

//
// alpaca_trade_client public methods

net::awaitable<std::expected<trade_account, alpaca_api_error>> alpaca_trade_client::account() const
{
    co_return co_await make_api_request<http::verb::get, trade_account>("/account");
}
net::awaitable<std::expected<std::vector<position>, alpaca_api_error>> alpaca_trade_client::all_open_positions() const
{
    co_return co_await make_api_request<http::verb::get, std::vector<position>>("/positions");
}
net::awaitable<std::expected<std::vector<position_closed>, alpaca_api_error>>
    alpaca_trade_client::close_all_positions(const bool cancel_orders) const
{
    const std::string endpoint{
        "/positions?cancel_orders=" + (cancel_orders ? std::string{"true"} : std::string{"false"})};
    co_return co_await make_api_request<http::verb::delete_, std::vector<position_closed>>(
        endpoint, http::status::multi_status);
}

net::awaitable<std::expected<std::vector<order>, alpaca_api_error>> alpaca_trade_client::get_all_orders() const
{
    co_return co_await make_api_request<http::verb::get, std::vector<order>>("/orders");
}

net::awaitable<std::expected<std::vector<order_deleted>, alpaca_api_error>>
    alpaca_trade_client::delete_all_orders() const
{
    co_return co_await make_api_request<http::verb::delete_, std::vector<order_deleted>>(
        "/orders", http::status::multi_status);
}

net::awaitable<std::expected<order, alpaca_api_error>> alpaca_trade_client::create_order(const notional_order& no) const
{
    const nlohmann::json no_json = no;

    http::fields extra_headers;
    extra_headers.set(http::field::content_type, "application/json");

    co_return co_await make_api_request<http::verb::post, order>(
        "/orders", no_json.dump(), http::status::ok, extra_headers);
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

// No body version
template<http::verb Verb, typename ReturnType>
net::awaitable<std::expected<ReturnType, alpaca_api_error>> alpaca_trade_client::make_api_request(
    const std::string&                 endpoint,
    const http::status                 expected_status,
    const std::optional<http::fields>& extra_headers) const
{
    co_return co_await make_api_request_impl<Verb, ReturnType, false>(
        endpoint, expected_status, std::nullopt, extra_headers);
}

// With body version
template<http::verb Verb, typename ReturnType>
net::awaitable<std::expected<ReturnType, alpaca_api_error>> alpaca_trade_client::make_api_request(
    const std::string&                 endpoint,
    const std::string&                 body,
    http::status                       expected_status,
    const std::optional<http::fields>& extra_headers) const
{
    co_return co_await make_api_request_impl<Verb, ReturnType, true>(endpoint, expected_status, body, extra_headers);
}

// Shared implementation
template<http::verb Verb, typename ReturnType, bool HasBody>
net::awaitable<std::expected<ReturnType, alpaca_api_error>> alpaca_trade_client::make_api_request_impl(
    const std::string&                 endpoint,
    http::status                       expected_status,
    const std::optional<std::string>&  body,
    const std::optional<http::fields>& extra_headers) const
{
    const std::string url{_cfg.base_url() + endpoint};
    auto              headers = create_auth_headers();

    if (extra_headers.has_value())
    {
        for (const auto& field : extra_headers.value())
        {
            headers.insert(field.name(), field.value());
        }
    }

    auto [ec, res] = co_await [&]()
    {
        if constexpr (HasBody)
        {
            return _rest_client->request<Verb>(url, headers, body.value());
        }
        else
        {
            return _rest_client->request<Verb>(url, headers);
        }
    }();

    if (ec)
    {
        co_return std::unexpected{alpaca_api_error{alpaca_api_error::error_type::network_error, 0, ec.message()}};
    }

    if (res.result() != expected_status)
    {
        co_return std::unexpected{
            alpaca_api_error{alpaca_api_error::error_type::http_error, static_cast<int>(res.result()), res.body()}};
    }

    try
    {
        const auto& body_str = res.body();

        // Debug: Log body characteristics
        std::cout << "DEBUG: Body size: " << body_str.size() << std::endl;
        std::cout << "DEBUG: First char: '" << (body_str.empty() ? '?' : body_str[0]) << "'" << std::endl;
        std::cout << "DEBUG: Last char: '" << (body_str.empty() ? '?' : body_str.back()) << "'" << std::endl;
        std::cout << "DEBUG: First 50 chars: " << body_str.substr(0, 50) << std::endl;

        const auto response_json{nlohmann::json::parse(body_str)};

        std::cout << "DEBUG: JSON type: "
                  << (response_json.is_object()  ? "object"
                      : response_json.is_array() ? "array"
                                                 : "other")
                  << std::endl;

        const ReturnType result = response_json;
        co_return result;
    }
    catch (const nlohmann::json::exception& e)
    {
        const std::string error_msg = std::string(e.what()) + " | Response body: " + res.body();
        co_return std::unexpected{alpaca_api_error{
            alpaca_api_error::error_type::json_parse_error, static_cast<int>(res.result()), error_msg}};
    }
}
