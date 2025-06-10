#pragma once

#include "detail/type_traits.hpp"
#include <boost/beast.hpp>

namespace async_rest_client {

namespace http = boost::beast::http;

template<typename Body>
struct response_parser {
    static_assert(detail::always_false_v<Body>, 
        "response_parser specialization not implemented for this body type. "
        "Available specializations: http::string_body");
};

}