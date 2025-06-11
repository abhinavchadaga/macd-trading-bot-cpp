#pragma once

#include "connection_context.hpp"

namespace async_rest_client
{

struct base_task
{
  virtual ~base_task()                            = default;
  virtual bool run(http_connection_context &ctx)  = 0;
  virtual bool run(https_connection_context &ctx) = 0;
};

} // namespace async_rest_client
