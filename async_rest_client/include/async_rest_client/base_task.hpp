#pragma once

#include "connection_context.hpp"

#include <boost/asio.hpp>
#include <boost/url.hpp>

namespace async_rest_client
{

namespace net = boost::asio;

struct base_task
{
  virtual ~base_task()                                            = default;
  virtual net::awaitable<bool> run(http_connection_context &ctx)  = 0;
  virtual net::awaitable<bool> run(https_connection_context &ctx) = 0;
  [[nodiscard]]
  virtual const boost::url &endpoint() const
    = 0;
};

} // namespace async_rest_client
