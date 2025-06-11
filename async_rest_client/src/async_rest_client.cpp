#include "async_rest_client/async_rest_client.hpp"

#include <memory>

namespace async_rest_client
{

std::shared_ptr<async_rest_client>
async_rest_client::create(net::io_context &ioc)
{
  return std::make_shared<async_rest_client>(ioc);
}

async_rest_client::async_rest_client(net::io_context &ioc)
  : _ioc { ioc }
  , _ssl_ctx { ssl::context::tlsv12_client }
  , _tcp_stream { _ioc }
  , _ssl_stream { _ioc, _ssl_ctx }
{
}

void
async_rest_client::connect(const std::string &url)
{
  // TODO: Implement in Milestone 2
}

} // namespace async_rest_client
