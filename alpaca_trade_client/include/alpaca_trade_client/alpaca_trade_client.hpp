//
// Created by Abhinav  Chadaga on 6/19/25.
//

#pragma once
#include "async_rest_client/async_rest_client.hpp"

#include <memory>

namespace net = boost::asio;

class alpaca_trade_client
{
public:
    class config
    {
    public:
        config(std::string api_key, std::string api_secret, bool paper_trading = true);

    private:
        std::string _api_key{};
        std::string _api_secret{};
        std::string _base_url{};
    };

    static std::shared_ptr<alpaca_trade_client> create(net::io_context& ioc, config config);

private:
    explicit alpaca_trade_client(net::io_context& ioc, config cfg);

private:
    config                                                _cfg;
    std::shared_ptr<async_rest_client::async_rest_client> _rest_client{};
};
