#pragma once

#include <boost/beast.hpp>
#include <boost/url.hpp>
#include <functional>

namespace async_rest_client::detail
{

namespace http = boost::beast::http;

template <typename Derived, typename Body>
class request_builder_base
{
public:

  using request_type = http::request<Body>;
  using writer_type  = std::function<void(request_type &)>;

protected:

  static void setup_default_headers(
    request_type       &req,
    const boost::url   &url,
    const http::fields &header_params,
    const std::string  &user_agent);

  request_builder_base() = default;
};

template <typename Derived, typename Body>
inline void
request_builder_base<Derived, Body>::setup_default_headers(
  request_type       &req,
  const boost::url   &url,
  const http::fields &header_params,
  const std::string  &user_agent)
{
  req.version(11);
  req.target(url.encoded_target().empty() ? "/" : url.encoded_target());
  req.set(http::field::host, url.host());
  req.set(http::field::user_agent, user_agent);
  req.set(http::field::connection, "keep-alive");

  for (const auto &field : header_params)
    {
      if (field.name() == http::field::unknown)
        {
          req.insert(field.name_string(), field.value());
        }
      else
        {
          req.set(field.name(), field.value());
        }
    }
}

} // namespace async_rest_client::detail
