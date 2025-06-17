#pragma once

#include <boost/beast/http/verb.hpp>
#include <boost/url.hpp>
#include <string_view>

namespace http = boost::beast::http;

namespace async_rest_client
{
/**
 * @param url_sv url to normalize
 * @return url with default port if port is missing based off of scheme
 */
boost::url make_http_https_url(const std::string_view& url_sv);

template<http::verb Verb>
constexpr std::string_view http_verb_to_string() noexcept
{
    using http::verb;

    if constexpr (Verb == verb::get)
    {
        return "GET";
    }
    else if constexpr (Verb == verb::post)
    {
        return "POST";
    }
    else if constexpr (Verb == verb::delete_)
    {
        return "DELETE";
    }
    else if constexpr (Verb == verb::patch)
    {
        return "PATCH";
    }
    else
    {
        static_assert(0, "Unsupported HTTP verb");
    }

    return "";
}

} // namespace async_rest_client
