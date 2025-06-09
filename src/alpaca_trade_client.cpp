#include "alpaca_trade_client.hpp"

//
// Paper trading endpoint for now

const char *alpaca_trade_client::ENDPOINT {
  "https://paper-api.alpaca.markets"
};

//
// Public Static Methods

std::shared_ptr<alpaca_trade_client>
alpaca_trade_client::create(net::io_context &ioc, ssl::context &ctxt)
{
  return std::shared_ptr<alpaca_trade_client>(
    new alpaca_trade_client(ioc, ctxt));
}

//
// Private Methods

alpaca_trade_client::alpaca_trade_client(
  net::io_context &ioc,
  ssl::context    &ctxt)
  : _resolver { ioc }
  , _stream { ioc, ctxt }
{
}
