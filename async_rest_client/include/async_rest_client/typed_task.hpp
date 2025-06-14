#pragma once

#include "base_task.hpp"
#include "concepts.hpp"
#include "utils.hpp"

#include <boost/asio/any_completion_handler.hpp>
#include <boost/beast.hpp>
#include <boost/url.hpp>
#include <string_view>

namespace async_rest_client
{

namespace net   = boost::asio;
namespace beast = boost::beast;
namespace http  = beast::http;

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
class typed_task final : public base_task
{
public:

  typed_task(
    net::io_context::executor_type executor,
    std::string_view               url,
    http::verb                     verb,
    http::fields                   headers,
    typename ReqBody::value_type   request_payload);

  //
  // Start of base_task methods

  net::awaitable<bool> send(http_connection_context &ctx) override;
  net::awaitable<bool> send(https_connection_context &ctx) override;
  void                 fail(boost::system::error_code ec) override;

  //
  // Start of typed_task methods

  net::awaitable<
    std::tuple<boost::system::error_code, http::response<ResBody>>>
  async_wait();

  [[nodiscard]]
  const boost::url &endpoint() const override;

private:

  template <SupportedStreamType Stream>
  net::awaitable<bool> run_impl(Stream &stream, beast::flat_buffer &buffer);

  net::any_completion_handler<
    void(boost::system::error_code, http::response<ResBody>)>
                                 _handler {};
  net::io_context::executor_type _executor;
  boost::url                     _endpoint {};
  http::verb                     _verb {};
  http::fields                   _headers {};
  typename ReqBody::value_type   _request_payload {};
};

//
// Implementations

//
// Ctor

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
typed_task<ReqBody, ResBody>::typed_task(
  net::io_context::executor_type executor,
  const std::string_view         url,
  const http::verb               verb,
  http::fields                   headers,
  typename ReqBody::value_type   request_payload)
  : _executor { std::move(executor) }
  , _endpoint { make_http_https_url(url) }
  , _verb { verb }
  , _headers { std::move(headers) }
  , _request_payload { std::move(request_payload) }
{
}

//
// Start of public methods

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
net::awaitable<bool>
typed_task<ReqBody, ResBody>::send(http_connection_context &ctx)
{
  co_return co_await run_impl(ctx.stream, ctx.buffer);
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
net::awaitable<bool>
typed_task<ReqBody, ResBody>::send(https_connection_context &ctx)
{
  co_return co_await run_impl(ctx.stream, ctx.buffer);
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
void
typed_task<ReqBody, ResBody>::fail(boost::system::error_code ec)
{
  if (_handler)
    {
      std::move(_handler)(ec, http::response<ResBody> {});
    }
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
net::awaitable<std::tuple<boost::system::error_code, http::response<ResBody>>>
typed_task<ReqBody, ResBody>::async_wait()
{
  auto [ec, response] = co_await net::async_initiate<
    void(boost::system::error_code, http::response<ResBody>)>(
    net::as_tuple(net::use_awaitable),
    [this]<typename Handler>(Handler &&handler) {
      _handler = [w = net::make_work_guard(_executor),
                  h = std::forward<Handler>(
                    handler)](auto error_code, auto response) mutable {
        std::move(h)(error_code, std::move(response));
      };
    });

  co_return std::make_tuple(ec, std::move(response));
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
const boost::url &
typed_task<ReqBody, ResBody>::endpoint() const
{
  return _endpoint;
}

//
// Start of private methods

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
template <SupportedStreamType Stream>
net::awaitable<bool>
typed_task<ReqBody, ResBody>::run_impl(
  Stream             &stream,
  beast::flat_buffer &buffer)
{
  try
    {
      http::request<ReqBody> req { _verb, _endpoint.encoded_target(), 11 };
      req.set(http::field::host, _endpoint.encoded_host());
      req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

      for (const auto &header : _headers)
        req.set(header.name_string(), header.value());

      req.set(http::field::connection, "keep-alive");

      req.body() = std::move(_request_payload);
      req.prepare_payload();

      co_await http::async_write(stream, req, net::use_awaitable);

      http::response<ResBody> res {};
      co_await http::async_read(stream, buffer, res, net::use_awaitable);
      buffer.consume(buffer.size());

      assert(_handler && "Handler must be set before run_impl is called");
      std::move(_handler)(boost::system::error_code {}, std::move(res));
      co_return true;
    }
  catch (const boost::system::system_error &e)
    {
      // TODO: log run error here
      std::move(_handler)(e.code(), http::response<ResBody> {});
      co_return false;
    }
  catch (const std::exception &)
    {
      // TODO: log run error here
      boost::system::error_code ec { boost::system::errc::io_error,
                                     boost::system::generic_category() };
      std::move(_handler)(ec, http::response<ResBody> {});
      co_return false;
    }
}
} // namespace async_rest_client
