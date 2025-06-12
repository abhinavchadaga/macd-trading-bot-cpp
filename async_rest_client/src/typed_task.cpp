#include "async_rest_client/typed_task.hpp"

#include "async_rest_client/utils.hpp"

#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/version.hpp>

namespace async_rest_client
{

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
typed_task<ReqBody, ResBody>::typed_task(
  const std::string_view       url,
  const http::verb             verb,
  http::fields                 headers,
  typename ReqBody::value_type request_payload)
  : _endpoint { make_http_https_url(url) }
  , _verb { verb }
  , _headers { std::move(headers) }
  , _request_payload { std::move(request_payload) }
{
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
net::awaitable<bool>
typed_task<ReqBody, ResBody>::run(http_connection_context &ctx)
{
  co_return co_await run_impl(ctx.stream, ctx.buffer);
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
net::awaitable<bool>
typed_task<ReqBody, ResBody>::run(https_connection_context &ctx)
{
  co_return co_await run_impl(ctx.stream, ctx.buffer);
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
const boost::url &
typed_task<ReqBody, ResBody>::endpoint() const
{
  return _endpoint;
}

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
        {
          req.set(header.name(), header.value());
        }

      req.set(http::field::connection, "keep-alive");

      req.body() = _request_payload;
      req.prepare_payload();

      co_await http::async_write(stream, req, net::use_awaitable);

      http::response<ResBody> res;
      co_await http::async_read(stream, buffer, res, net::use_awaitable);
      buffer.consume(buffer.size());

      _promise.set_value(std::move(res));
      co_return true;
    }
  catch (const std::exception &)
    {
      _promise.set_exception(std::current_exception());
      co_return false;
    }
}

template class typed_task<http::empty_body, http::string_body>;
template class typed_task<http::string_body, http::string_body>;

} // namespace async_rest_client
