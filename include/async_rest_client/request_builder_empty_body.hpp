#pragma once

#include "request_builder_fwd.hpp"

#include <boost/beast.hpp>
#include <boost/url.hpp>

namespace async_rest_client
{

namespace http = boost::beast::http;

template <>
struct request_builder<http::empty_body>
  : public detail::
      request_builder_base<request_builder<http::empty_body>, http::empty_body>
{
  static writer_type build(
    const boost::url   &url,
    const http::fields &header_params,
    const std::string  &user_agent);
};

//
// Template Implementation

inline request_builder<http::empty_body>::writer_type
request_builder<http::empty_body>::build(
  const boost::url   &url,
  const http::fields &header_params,
  const std::string  &user_agent)
{
  return [&url, header_params, user_agent](request_type &req) {
    req.method(http::verb::get);
    setup_default_headers(req, url, header_params, user_agent);
  };
}

} // namespace async_rest_client
