#include "async_rest_client/async_rest_client.hpp"
#include "LoggingUtils.hpp"
#include <boost/url.hpp>

namespace async_rest_client {

std::shared_ptr<async_rest_client> async_rest_client::create(
    net::io_context& ioc,
    ssl::context& ssl_ctx,
    const config& cfg
) {
    return std::shared_ptr<async_rest_client>(new async_rest_client(ioc, ssl_ctx, cfg));
}

async_rest_client::async_rest_client(
    net::io_context& ioc,
    ssl::context& ssl_ctx,
    const config& cfg
) : _ioc{ioc},
    _ssl_ctx{ssl_ctx},
    _config{cfg},
    _strand{net::make_strand(ioc)},
    _resolver{_strand},
    _stream{_strand, ssl_ctx},
    _timer{_strand}
{
    LOG_INFO(async_rest_client, constructor);
}

void async_rest_client::enqueue_task(std::shared_ptr<detail::base_task> task) {
    LOG_DEBUG(async_rest_client, enqueue_task);
    
    net::post(_strand, [self = shared_from_this(), task]() {
        self->_task_queue.push(task);
        self->process_request_queue();
    });
}

void async_rest_client::process_request_queue() {
    LOG_DEBUG(async_rest_client, process_request_queue);
    
    if (_request_in_progress || _task_queue.empty() || !_connected) {
        return;
    }
    
    send_next_request();
}

void async_rest_client::send_next_request() {
    LOG_DEBUG(async_rest_client, send_next_request);
    
    if (_task_queue.empty()) {
        return;
    }
    
    _current_task = _task_queue.front();
    _task_queue.pop();
    _request_in_progress = true;
    
    start_timeout();
    
    _current_task->write(_stream, [self = shared_from_this()](boost::system::error_code ec) {
        self->on_write(ec);
    });
}

void async_rest_client::on_write(boost::system::error_code ec) {
    cancel_timeout();
    
    if (ec) {
        LOG_ERROR(async_rest_client, on_write);
        _request_in_progress = false;
        _current_task.reset();
        process_request_queue();
        return;
    }
    
    LOG_DEBUG(async_rest_client, on_write);
    start_timeout();
    
    _current_task->read(_stream, _buffer, [self = shared_from_this()](boost::system::error_code ec) {
        self->on_read(ec);
    });
}

void async_rest_client::on_read(boost::system::error_code ec) {
    cancel_timeout();
    
    if (ec) {
        LOG_ERROR(async_rest_client, on_read);
    } else {
        LOG_DEBUG(async_rest_client, on_read);
    }
    
    _request_in_progress = false;
    _current_task.reset();
    _buffer.clear();
    
    process_request_queue();
}

void async_rest_client::connect(const std::string& host, const std::string& port, completion_handler handler) {
    if (_connecting || _connected) {
        LOG_WARNING(async_rest_client, connect);
        net::post(_strand, [handler]() {
            handler(boost::system::errc::make_error_code(boost::system::errc::already_connected), "");
        });
        return;
    }

    LOG_INFO(async_rest_client, connect);
    _connecting = true;
    _host = host;
    _port = port;

    start_timeout();

    if (!SSL_set_tlsext_host_name(_stream.native_handle(), host.c_str())) {
        boost::system::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        LOG_ERROR(async_rest_client, connect);
        _connecting = false;
        cancel_timeout();
        handler(ec, "");
        return;
    }

    _resolver.async_resolve(
        host,
        port,
        [self = shared_from_this(), handler](auto ec, auto results) {
            self->on_resolve(ec, results, handler);
        }
    );
}

void async_rest_client::on_resolve(
    boost::system::error_code ec,
    net::ip::tcp::resolver::results_type results,
    completion_handler handler
) {
    if (ec) {
        LOG_ERROR(async_rest_client, on_resolve);
        _connecting = false;
        cancel_timeout();
        handler(ec, "");
        return;
    }

    LOG_DEBUG(async_rest_client, on_resolve);
    beast::get_lowest_layer(_stream).async_connect(
        results,
        [self = shared_from_this(), handler](auto ec, auto endpoint) {
            self->on_connect(ec, endpoint, handler);
        }
    );
}

void async_rest_client::on_connect(
    boost::system::error_code ec,
    net::ip::tcp::resolver::results_type::endpoint_type,
    completion_handler handler
) {
    if (ec) {
        LOG_ERROR(async_rest_client, on_connect);
        _connecting = false;
        cancel_timeout();
        handler(ec, "");
        return;
    }

    LOG_DEBUG(async_rest_client, on_connect);
    _stream.async_handshake(
        ssl::stream_base::client,
        [self = shared_from_this(), handler](auto ec) {
            self->on_handshake(ec, handler);
        }
    );
}

void async_rest_client::on_handshake(
    boost::system::error_code ec,
    completion_handler handler
) {
    _connecting = false;
    cancel_timeout();

    if (ec) {
        LOG_ERROR(async_rest_client, on_handshake);
        handler(ec, "");
        return;
    }

    LOG_INFO(async_rest_client, on_handshake);
    _connected = true;
    handler({}, "Connected");
    
    process_request_queue();
}

void async_rest_client::disconnect() {
    LOG_INFO(async_rest_client, disconnect);
    
    if (!_connected && !_connecting) {
        return;
    }

    cancel_timeout();
    _connected = false;
    _connecting = false;
    _request_in_progress = false;
    
    while (!_task_queue.empty()) {
        _task_queue.pop();
    }
    _current_task.reset();

    boost::system::error_code ec;
    _stream.shutdown(ec);
    if (ec) {
        LOG_WARNING(async_rest_client, disconnect);
    }

    beast::get_lowest_layer(_stream).close();
}

bool async_rest_client::is_connected() const {
    return _connected;
}

void async_rest_client::start_timeout() {
    _timer.expires_after(_config.timeout);
    _timer.async_wait([self = shared_from_this()](auto ec) {
        self->on_timeout(ec);
    });
}

void async_rest_client::cancel_timeout() {
    boost::system::error_code ec;
    _timer.cancel(ec);
}

void async_rest_client::on_timeout(boost::system::error_code ec) {
    if (ec == net::error::operation_aborted) {
        return;
    }

    if (ec) {
        LOG_ERROR(async_rest_client, on_timeout);
        return;
    }

    LOG_WARNING(async_rest_client, on_timeout);
    disconnect();
}

}