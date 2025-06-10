#pragma once

#include "base_task.hpp"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <memory>

namespace async_rest_client {
namespace detail {

namespace beast = boost::beast;
namespace net = boost::asio;
namespace http = beast::http;

template<typename RequestBody, typename ResponseBody>
class typed_task : public base_task, public std::enable_shared_from_this<typed_task<RequestBody, ResponseBody>> {
public:
    using request_type = http::request<RequestBody>;
    using response_type = http::response<ResponseBody>;
    using user_completion_handler = std::function<void(boost::system::error_code, const std::string&)>;
    using request_writer = std::function<void(request_type&)>;
    using response_reader = std::function<void(const response_type&, boost::system::error_code, user_completion_handler)>;

    static std::shared_ptr<typed_task> create(
        request_writer writer,
        response_reader reader,
        user_completion_handler handler
    ) {
        return std::shared_ptr<typed_task>(new typed_task(std::move(writer), std::move(reader), std::move(handler)));
    }

    void write(
        beast::ssl_stream<beast::tcp_stream>& stream,
        std::function<void(boost::system::error_code)> completion_handler
    ) override {
        _request_writer(_request);

        http::async_write(
            stream,
            _request,
            [completion_handler](boost::system::error_code ec, std::size_t) {
                completion_handler(ec);
            }
        );
    }

    void read(
        beast::ssl_stream<beast::tcp_stream>& stream,
        beast::flat_buffer& buffer,
        std::function<void(boost::system::error_code)> completion_handler
    ) override {
        http::async_read(
            stream,
            buffer,
            _response,
            [self = this->shared_from_this(), completion_handler](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    self->_user_handler(ec, "");
                } else {
                    self->_response_reader(self->_response, ec, self->_user_handler);
                }
                completion_handler(ec);
            }
        );
    }

private:
    typed_task(
        request_writer writer,
        response_reader reader,
        user_completion_handler handler
    ) : _request_writer{std::move(writer)},
        _response_reader{std::move(reader)},
        _user_handler{std::move(handler)}
    {}

    request_type _request;
    response_type _response;
    request_writer _request_writer;
    response_reader _response_reader;
    user_completion_handler _user_handler;
};

}
}