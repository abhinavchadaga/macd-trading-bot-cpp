#pragma once

#include "detail/request_builder_base.hpp"
#include "detail/type_traits.hpp"

#include <boost/beast.hpp>

namespace async_rest_client
{

namespace http = boost::beast::http;

template <typename Body>
struct request_builder
  : public detail::request_builder_base<request_builder<Body>, Body>
{
  static_assert(
    detail::always_false_v<Body>,
    "request_builder specialization not implemented for this body type. "
    "Available specializations: http::empty_body, http::string_body");
};

} // namespace async_rest_client
