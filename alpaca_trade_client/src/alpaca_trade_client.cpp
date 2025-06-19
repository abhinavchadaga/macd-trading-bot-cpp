//
// Created by Abhinav  Chadaga on 6/19/25.
//

#include "alpaca_trade_client/alpaca_trade_client.hpp"

//
// Config class

alpaca_trade_client::config::config(std::string api_key, std::string api_secret, const bool paper_trading)
    : _api_key{std::move(api_key)},
      _api_secret{std::move(api_secret)},
      _base_url{paper_trading ? "https://paper-api.alpaca.markets/v2" : "https://api.alpaca.markets"}
{
}

//
// alpaca_trade_client public static methods

std::shared_ptr<alpaca_trade_client> alpaca_trade_client::create(net::io_context& ioc, config config)
{
    return std::shared_ptr<alpaca_trade_client>(new alpaca_trade_client{ioc, std::move(config)});
}

//
// alpaca_trade_client private methods

alpaca_trade_client::alpaca_trade_client(net::io_context& ioc, config cfg)
    : _cfg{std::move(cfg)},
      _rest_client{async_rest_client::async_rest_client::create(ioc)}
{
}
