#pragma once

#include "detail/base_task.hpp"
#include "detail/typed_task.hpp"
#include "request_builder_empty_body.hpp"
#include "response_parser_fwd.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/url.hpp>
#include <functional>
#include <memory>
#include <queue>
#include <string>

namespace async_rest_client
{

namespace http  = boost::beast::http;
namespace net   = boost::asio;
namespace ssl   = boost::asio::ssl;
namespace beast = boost::beast;

class async_rest_client
  : public std::enable_shared_from_this<async_rest_client>
{
public:

  struct config
  {
    config();

    std::chrono::seconds timeout;
    bool                 verify_ssl;
    std::string          user_agent;
  };

  using req_completion_handler
    = std::function<void(boost::system::error_code, const std::string &)>;

  //
  // Create method

  static std::shared_ptr<async_rest_client> create(
    net::io_context &ioc,
    ssl::context    &ssl_ctx,
    const config    &cfg = config {});

  //
  // Connection methods

  void connect(
    const std::string     &host,
    const std::string     &port,
    req_completion_handler handler);
  void disconnect();
  bool is_connected() const;

  //
  // Verb methods

  template <typename ResponseBody = http::string_body>
  void get(
    const std::string &url,
    const http::fields &header_params,
    req_completion_handler handler);

  template <
    typename RequestBody  = http::string_body,
    typename ResponseBody = http::string_body>
  void post(
    const std::string &url,
    const http::fields &header_params,
    const std::string &body,
    std::function<void(boost::system::error_code, const std::string &)>
      completion_handler);

private:

  explicit async_rest_client(
    net::io_context &ioc,
    ssl::context    &ssl_ctx,
    config           cfg);

  void enqueue_task(std::shared_ptr<detail::base_task> task);
  void process_request_queue();
  void send_next_request();
  void on_write(boost::system::error_code ec);
  void on_read(boost::system::error_code ec);

  void on_resolve(
    boost::system::error_code            ec,
    net::ip::tcp::resolver::results_type results,
    req_completion_handler               handler);

  void on_connect(
    boost::system::error_code ec,
    net::ip::tcp::resolver::results_type::endpoint_type,
    req_completion_handler handler);

  void on_handshake(
    boost::system::error_code ec,
    req_completion_handler    handler);

  void start_timeout();
  void cancel_timeout();
  void on_timeout(boost::system::error_code ec);

  net::io_context &_ioc;
  ssl::context    &_ssl_ctx;
  config           _config;

  net::ip::tcp::resolver               _resolver;
  beast::ssl_stream<beast::tcp_stream> _stream;
  beast::flat_buffer                   _buffer;
  net::steady_timer                    _timer;

  std::string _host;
  std::string _port;
  bool        _connected { false };
  bool        _connecting { false };

  std::queue<std::shared_ptr<detail::base_task>> _task_queue;
  std::shared_ptr<detail::base_task>             _current_task;
  bool _request_in_progress { false };
};

template <typename ResponseBody>
inline void
async_rest_client::get(
  const std::string     &url,
  const http::fields    &header_params,
  req_completion_handler handler)
{
  auto writer = request_builder<http::empty_body>::build(
    url,
    header_params,
    _config.user_agent);

  auto reader = response_parser<ResponseBody>::create_reader();

  auto task = detail::typed_task<http::empty_body, ResponseBody>::create(
    writer,
    reader,
    handler);

  enqueue_task(std::static_pointer_cast<detail::base_task>(task));
}

} // namespace async_rest_client
