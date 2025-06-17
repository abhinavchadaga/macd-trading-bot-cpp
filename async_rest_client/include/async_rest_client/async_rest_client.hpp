#pragma once

#include "base_task.hpp"
#include "concepts.hpp"
#include "typed_task.hpp"
#include "utils.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/url.hpp>
#include <deque>
#include <memory>

namespace async_rest_client
{

namespace beast     = boost::beast;
namespace http      = beast::http;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
namespace ssl       = boost::asio::ssl;
using tcp           = boost::asio::ip::tcp;

class async_rest_client : public std::enable_shared_from_this<async_rest_client>
{
public:
    static std::shared_ptr<async_rest_client> create(net::io_context& ioc);

    ~async_rest_client();

    net::awaitable<boost::system::error_code> connect(std::string_view url_sv);

    template<
        http::verb Verb,
        typename RequestBody  = typename default_body_types<Verb>::request_body,
        typename ResponseBody = typename default_body_types<Verb>::response_body>
        requires ValidVerbBodyCombination<Verb, RequestBody, ResponseBody>
    net::awaitable<std::tuple<boost::system::error_code, http::response<ResponseBody>>>
        request(std::string_view url, http::fields headers = {}, typename RequestBody::value_type body = {});

private:
    enum class connection_state
    {
        NOT_CONNECTED,
        CONNECTING,
        CONNECTED
    };

    //
    // Private Ctor

    explicit async_rest_client(net::io_context& ioc);

    net::awaitable<void> process_queue();
    void                 enqueue_task(std::unique_ptr<base_task>&& task);
    net::awaitable<void> graceful_shutdown();

    //
    // Task helpers

    template<http::verb Verb, typename RequestBody, typename ResponseBody>
    std::unique_ptr<base_task>
        make_task(std::string_view url, http::fields headers = {}, typename RequestBody::value_type body = {});

    std::deque<std::unique_ptr<base_task>> _tasks;
    bool                                   _is_processing{false};

    net::io_context&                                      _ioc;
    ssl::context                                          _ssl_ctx;
    std::unique_ptr<beast::tcp_stream>                    _tcp_stream;
    std::unique_ptr<beast::ssl_stream<beast::tcp_stream>> _ssl_stream;
    std::unique_ptr<beast::flat_buffer>                   _buffer;

    boost::url       _current_origin{};
    connection_state _connection_state{connection_state::NOT_CONNECTED};
    bool             _is_tls{false};
};

//
// Template Implementations

template<http::verb Verb, typename RequestBody, typename ResponseBody>
    requires ValidVerbBodyCombination<Verb, RequestBody, ResponseBody>
net::awaitable<std::tuple<boost::system::error_code, http::response<ResponseBody>>>
    async_rest_client::request(std::string_view url, http::fields headers, typename RequestBody::value_type body)
{
    LOG_INFO("making {} request to {}", http_verb_to_string<Verb>(), url);

    auto task{make_task<Verb, RequestBody, ResponseBody>(std::move(url), std::move(headers), std::move(body))};
    typed_task<RequestBody, ResponseBody>* task_ptr{static_cast<typed_task<RequestBody, ResponseBody>*>(task.get())};
    enqueue_task(std::move(task));

    auto [ec, response]{co_await task_ptr->async_wait()};
    co_return std::make_tuple(ec, std::move(response));
}

template<http::verb Verb, typename RequestBody, typename ResponseBody>
std::unique_ptr<base_task>
    async_rest_client::make_task(std::string_view url, http::fields headers, typename RequestBody::value_type body)
{
    return std::make_unique<typed_task<RequestBody, ResponseBody>>(
        _ioc.get_executor(), Verb, std::move(url), std::move(headers), std::move(body));
}

} // namespace async_rest_client
