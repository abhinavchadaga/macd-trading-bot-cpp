#pragma once

#include "request_builder_fwd.hpp"
#include <boost/beast.hpp>
#include <boost/url.hpp>
#include <functional>

namespace async_rest_client {

namespace http = boost::beast::http;

template<>
struct request_builder<http::string_body> {
    using request_type = http::request<http::string_body>;
    using writer_type = std::function<void(request_type&)>;

    static writer_type build(
        const std::string& url,
        const http::fields& header_params,
        const std::string& user_agent,
        const std::string& body
    ) {
        return [url, header_params, user_agent, body](request_type& req) {
            boost::urls::url_view parsed_url(url);
            
            req.method(http::verb::post);
            req.target(parsed_url.path().empty() ? "/" : parsed_url.path());
            req.version(11);
            req.set(http::field::host, parsed_url.host());
            req.set(http::field::user_agent, user_agent);
            req.set(http::field::connection, "close");
            req.set(http::field::content_type, "application/json");
            
            for (const auto& field : header_params) {
                if (field.name() == http::field::unknown) {
                    req.insert(field.name_string(), field.value());
                } else {
                    req.set(field.name(), field.value());
                }
            }
            
            req.body() = body;
            req.prepare_payload();
        };
    }
};

}