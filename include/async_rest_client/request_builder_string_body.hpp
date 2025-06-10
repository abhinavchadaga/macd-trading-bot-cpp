#pragma once

#include "request_builder_fwd.hpp"

#include <boost/beast.hpp>
#include <boost/url.hpp>

namespace async_rest_client
{

namespace http = boost::beast::http;

template <>
struct request_builder<http::string_body>
  : public detail::request_builder_base<
      request_builder<http::string_body>,
      http::string_body>
{
  static writer_type build(
    const boost::url   &url,
    const http::fields &header_params,
    const std::string  &user_agent,
    const std::string  &body);
};

//
// Template Implementation

inline request_builder<http::string_body>::writer_type
request_builder<http::string_body>::build(
  const boost::url   &url,
  const http::fields &header_params,
  const std::string  &user_agent,
  const std::string  &body)
{
  return [&url, header_params, user_agent, body](request_type &req) {
    req.method(http::verb::post);
    setup_default_headers(req, url, header_params, user_agent);
    req.set(http::field::content_type, "application/json");
    req.body() = body;
    req.prepare_payload();
  };
}

} // namespace async_rest_client
