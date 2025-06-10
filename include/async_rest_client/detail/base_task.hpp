#pragma once

#include <boost/beast.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/system/error_code.hpp>
#include <boost/url.hpp>
#include <functional>

namespace async_rest_client::detail
{

namespace beast = boost::beast;

class base_task
{
public:

  virtual ~base_task() = default;

  [[nodiscard]]
  virtual const boost::url &url() const
    = 0;

  virtual void write(
    beast::ssl_stream<beast::tcp_stream>          &stream,
    std::function<void(boost::system::error_code)> completion_handler)
    = 0;

  virtual void read(
    beast::ssl_stream<beast::tcp_stream>          &stream,
    beast::flat_buffer                            &buffer,
    std::function<void(boost::system::error_code)> completion_handler)
    = 0;
};

} // namespace async_rest_client::detail
