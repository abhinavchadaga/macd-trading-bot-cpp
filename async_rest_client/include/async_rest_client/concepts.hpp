#pragma once

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <concepts>

namespace async_rest_client
{

namespace beast = boost::beast;
namespace http  = beast::http;

template <typename T>
concept SupportedStreamType
  = std::same_as<T, beast::tcp_stream>
 || std::same_as<T, beast::ssl_stream<beast::tcp_stream>>;

template <typename Body>
concept SupportedRequestBody = std::same_as<Body, http::empty_body>
                            || std::same_as<Body, http::string_body>;

template <typename Body>
concept SupportedResponseBody = std::same_as<Body, http::string_body>;

} // namespace async_rest_client
