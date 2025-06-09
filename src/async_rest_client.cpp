#include "async_rest_client.hpp"

#include <memory>
#include <utility>

//
// Private methods

std::shared_ptr<async_rest_client>
async_rest_client::create(
  const std::string &name,
  net::io_context   &ioc,
  ssl::context      &ctxt)
{
  return std::shared_ptr<async_rest_client>(
    new async_rest_client { name, ioc, ctxt });
}

async_rest_client::async_rest_client(
  std::string      name,
  net::io_context &ioc,
  ssl::context    &ctxt)
  : _name { std::move(name) }
  , _resolver { ioc }
  , _stream { ioc, ctxt }
{
}
