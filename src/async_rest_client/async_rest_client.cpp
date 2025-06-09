#include "async_rest_client/async_rest_client.hpp"

#include <memory>
#include <utility>

//
// Static functions

std::shared_ptr<async_rest_client>
async_rest_client::create(
  const std::string &name,
  net::io_context   &ioc,
  ssl::context      &ctxt)
{
  return std::shared_ptr<async_rest_client>(
    new async_rest_client { name, ioc, ctxt });
}

//
// Private methods


async_rest_client::async_rest_client(
  std::string      name,
  net::io_context &ioc,
  ssl::context    &ctxt)
  : _name { std::move(name) }
  , _resolver { ioc }
  , _stream { ioc, ctxt }
{
}

int
main()
{
  net::io_context ioc;
  ssl::context    ctxt { ssl::context::tlsv12_client };
  ctxt.set_default_verify_paths();
  ctxt.set_verify_mode(ssl::verify_none);

  auto client { async_rest_client::create("test", ioc, ctxt) };
  ioc.run();
  return 0;
}
