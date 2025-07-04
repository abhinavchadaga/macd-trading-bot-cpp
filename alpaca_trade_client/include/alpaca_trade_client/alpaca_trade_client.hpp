//
// Created by Abhinav  Chadaga on 6/19/25.
//

#pragma once
#include "account.hpp"
#include "async_rest_client/async_rest_client.hpp"
#include "orders.hpp"
#include "position.hpp"

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <expected>
#include <memory>
#include <vector>

namespace net  = boost::asio;
namespace json = boost::json;

class alpaca_api_error
{
public:
    enum class error_type
    {
        network_error,
        http_error,
        json_parse_error,
    };

    alpaca_api_error(error_type type, int http_status, std::string message);

    [[nodiscard]] error_type         type() const;
    [[nodiscard]] int                http_status() const;
    [[nodiscard]] const std::string& message() const;

private:
    error_type  _type;
    int         _http_status;
    std::string _message;
};

class alpaca_trade_client
{
public:
    class config
    {
    public:
        //
        // Ctor
        config(std::string api_key, std::string api_secret, bool paper_trading = true);

        //
        // Accessors
        [[nodiscard]] std::string api_key() const;
        [[nodiscard]] std::string api_secret() const;
        [[nodiscard]] std::string base_url() const;

    private:
        std::string _api_key{};
        std::string _api_secret{};
        std::string _base_url{};
    };

    static std::shared_ptr<alpaca_trade_client> create(net::io_context& ioc, config config);

    //
    // /account

    [[nodiscard]] net::awaitable<std::expected<trade_account, alpaca_api_error>> account() const;

    //
    // /positions

    [[nodiscard]] net::awaitable<std::expected<std::vector<position>, alpaca_api_error>> all_open_positions() const;
    [[nodiscard]] net::awaitable<std::expected<std::vector<position_closed>, alpaca_api_error>>
        close_all_positions(bool cancel_orders = true) const;

    //
    // /orders

    [[nodiscard]] net::awaitable<std::expected<std::vector<order>, alpaca_api_error>>         get_all_orders() const;
    [[nodiscard]] net::awaitable<std::expected<std::vector<order_deleted>, alpaca_api_error>> delete_all_orders() const;
    [[nodiscard]] net::awaitable<std::expected<order, alpaca_api_error>> create_order(const notional_order& no) const;

private:
    explicit alpaca_trade_client(net::io_context& ioc, config cfg);

    [[nodiscard]] http::fields create_auth_headers() const;

    template<http::verb Verb, typename ReturnType>
    [[nodiscard]] net::awaitable<std::expected<ReturnType, alpaca_api_error>> make_api_request(
        const std::string&                 endpoint,
        http::status                       expected_status = http::status::ok,
        const std::optional<http::fields>& extra_headers   = std::nullopt) const;

    template<boost::beast::http::verb Verb, typename ReturnType>
    [[nodiscard]] net::awaitable<std::expected<ReturnType, alpaca_api_error>> make_api_request(
        const std::string&                 endpoint,
        const std::string&                 body,
        http::status                       expected_status = http::status::ok,
        const std::optional<http::fields>& extra_headers   = std::nullopt) const;

    template<http::verb Verb, typename ReturnType, bool HasBody>
    [[nodiscard]] net::awaitable<std::expected<ReturnType, alpaca_api_error>> make_api_request_impl(
        const std::string&                 endpoint,
        http::status                       expected_status,
        const std::optional<std::string>&  body,
        const std::optional<http::fields>& extra_headers) const;

private:
    config                                                _cfg;
    std::shared_ptr<async_rest_client::async_rest_client> _rest_client{};
};
