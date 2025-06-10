#pragma once

#include "response_parser_fwd.hpp"
#include <boost/beast.hpp>
#include <functional>

namespace async_rest_client {

namespace http = boost::beast::http;

template<>
struct response_parser<http::string_body> {
    using response_type = http::response<http::string_body>;
    using user_completion_handler = std::function<void(boost::system::error_code, const std::string&)>;
    using reader_type = std::function<void(const response_type&, boost::system::error_code, user_completion_handler)>;

    static reader_type create_reader() {
        return [](const response_type& res, boost::system::error_code ec, user_completion_handler handler) {
            if (ec) {
                handler(ec, "");
                return;
            }
            
            if (res.result() != http::status::ok) {
                boost::system::error_code error_ec{
                    static_cast<int>(res.result_int()), 
                    boost::system::generic_category()
                };
                handler(error_ec, res.body());
                return;
            }
            
            handler({}, res.body());
        };
    }
};

}