//
// Created by Abhinav  Chadaga on 6/19/25.
//

#pragma once
#include "async_rest_client/async_rest_client.hpp"

#include <boost/beast/http.hpp>
#include <expected>
#include <memory>
#include <nlohmann/json.hpp>

namespace net = boost::asio;

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
    [[nodiscard]] net::awaitable<std::expected<nlohmann::basic_json<>, alpaca_api_error>> account() const;

    //
    // /positions

    net::awaitable<std::expected<nlohmann::basic_json<>, alpaca_api_error>> all_open_positions() const;

private:
    explicit alpaca_trade_client(net::io_context& ioc, config cfg);

    [[nodiscard]] http::fields create_auth_headers() const;

private:
    config                                                _cfg;
    std::shared_ptr<async_rest_client::async_rest_client> _rest_client{};
};
