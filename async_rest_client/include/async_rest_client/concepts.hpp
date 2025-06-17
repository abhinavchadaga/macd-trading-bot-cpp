#pragma once

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <concepts>

namespace async_rest_client
{

namespace beast = boost::beast;
namespace http  = beast::http;

template<typename T>
concept SupportedStreamType =
    std::same_as<T, beast::tcp_stream> || std::same_as<T, beast::ssl_stream<beast::tcp_stream>>;

template<typename Body>
concept SupportedRequestBody = std::same_as<Body, http::empty_body> || std::same_as<Body, http::string_body>;

template<typename Body>
concept SupportedResponseBody = std::same_as<Body, http::string_body>;

template<http::verb>
struct default_body_types
{
    using request_body  = http::empty_body;
    using response_body = http::string_body;
};

template<>
struct default_body_types<http::verb::post>
{
    using request_body  = http::string_body;
    using response_body = http::string_body;
};

template<>
struct default_body_types<http::verb::patch>
{
    using request_body  = http::string_body;
    using response_body = http::string_body;
};

template<http::verb Verb, typename RequestBody, typename ResponseBody>
concept ValidVerbBodyCombination =
    ((Verb == http::verb::get || Verb == http::verb::delete_) && std::same_as<RequestBody, http::empty_body> &&
     std::same_as<ResponseBody, http::string_body>) ||
    ((Verb == http::verb::post || Verb == http::verb::patch) && std::same_as<RequestBody, http::string_body> &&
     std::same_as<ResponseBody, http::string_body>);

} // namespace async_rest_client
