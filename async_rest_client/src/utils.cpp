#include "async_rest_client/utils.hpp"

#include <stdexcept>

namespace async_rest_client
{

boost::url
make_http_https_url(const std::string_view &url_sv)
{
  boost::url url { url_sv };
  if (url.scheme() != "https" && url.scheme() != "http")
    throw std::invalid_argument("url must have scheme of http or https");

  if (!url.has_port())
    url.set_port(url.scheme() == "https" ? "443" : "80");

  return url;
}
} // namespace async_rest_client
