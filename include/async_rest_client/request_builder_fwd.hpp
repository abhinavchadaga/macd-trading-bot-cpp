#pragma once

#include "detail/type_traits.hpp"
#include <boost/beast.hpp>

namespace async_rest_client {

namespace http = boost::beast::http;

template<typename Body>
struct request_builder {
    static_assert(detail::always_false_v<Body>, 
        "request_builder specialization not implemented for this body type. "
        "Available specializations: http::empty_body, http::string_body");
};

}