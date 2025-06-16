#pragma once

#include <boost/system/system_error.hpp>
#include <boost/url.hpp>
#include <string>

namespace async_rest_client
{
/**
 * @param url_sv url to normalize
 * @return url with default port if port is missing based off of scheme
 */
boost::url make_http_https_url(const std::string_view& url_sv);

} // namespace async_rest_client
