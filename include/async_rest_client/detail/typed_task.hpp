#pragma once

#include "base_task.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <functional>
#include <memory>
#include <string>

namespace async_rest_client::detail
{

namespace beast = boost::beast;
namespace net   = boost::asio;
namespace http  = beast::http;

template <typename RequestBody, typename ResponseBody>
class typed_task
  : public base_task
  , public std::enable_shared_from_this<typed_task<RequestBody, ResponseBody>>
{
public:

  using request_type  = http::request<RequestBody>;
  using response_type = http::response<ResponseBody>;
  using user_completion_handler
    = std::function<void(boost::system::error_code, const std::string &)>;
  using request_writer  = std::function<void(request_type &)>;
  using response_reader = std::function<void(
    const response_type &,
    boost::system::error_code,
    user_completion_handler)>;

  static std::shared_ptr<typed_task>
  create(
    const std::string      &url,
    request_writer          writer,
    response_reader         reader,
    user_completion_handler handler)
  {
    return std::shared_ptr<typed_task>(new typed_task(
      url,
      std::move(writer),
      std::move(reader),
      std::move(handler)));
  }

  [[nodiscard]]
  const boost::url &
  url() const override
  {
    return _url;
  }

  void
  on_error(boost::system::error_code ec) override
  {
    _user_handler(ec, "");
  }

  void
  write(
    beast::ssl_stream<beast::tcp_stream>          &stream,
    std::function<void(boost::system::error_code)> on_write) override
  {
    _request_writer(_request);
    http::async_write(
      stream,
      _request,
      [on_write
       = std::move(on_write)](boost::system::error_code ec, std::size_t) {
        on_write(ec);
      });
  }

  void
  read(
    beast::ssl_stream<beast::tcp_stream>          &stream,
    beast::flat_buffer                            &buffer,
    std::function<void(boost::system::error_code)> on_read) override
  {
    http::async_read(
      stream,
      buffer,
      _response,
      [self = this->shared_from_this(),
       on_read
       = std::move(on_read)](boost::system::error_code ec, std::size_t) {
        if (ec)
          {
            self->_user_handler(ec, "");
            return;
          }
        else
          {
            self->_response_reader(self->_response, ec, self->_user_handler);
            on_read(ec);
          }
      });
  }

private:

  typed_task(
    const std::string      &url,
    request_writer          writer,
    response_reader         reader,
    user_completion_handler handler)
    : _url { url }
    , _request_writer { std::move(writer) }
    , _response_reader { std::move(reader) }
    , _user_handler { std::move(handler) }
  {
  }

  boost::url              _url;
  request_type            _request;
  response_type           _response;
  request_writer          _request_writer;
  response_reader         _response_reader;
  user_completion_handler _user_handler;
};

} // namespace async_rest_client::detail
