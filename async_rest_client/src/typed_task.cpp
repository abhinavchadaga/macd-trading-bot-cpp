#include "async_rest_client/typed_task.hpp"

namespace async_rest_client
{

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
typed_task<ReqBody, ResBody>::typed_task(
  std::string_view             url,
  http::verb                   verb,
  http::fields                 headers,
  typename ReqBody::value_type request_payload)
  : _endpoint { url }
  , _verb { verb }
  , _headers { std::move(headers) }
  , _request_payload { std::move(request_payload) }
{
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
bool
typed_task<ReqBody, ResBody>::run(http_connection_context &ctx)
{
  return run_impl(ctx.stream, ctx.buffer);
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
bool
typed_task<ReqBody, ResBody>::run(https_connection_context &ctx)
{
  return run_impl(ctx.stream, ctx.buffer);
}

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
template <SupportedStreamType Stream>
bool
typed_task<ReqBody, ResBody>::run_impl(
  Stream             &stream,
  beast::flat_buffer &buffer)
{
  // TODO: Implement in Milestone 4/5
  return true;
}

// Explicit template instantiations for supported combinations
template class typed_task<http::empty_body, http::string_body>;
template class typed_task<http::string_body, http::string_body>;

} // namespace async_rest_client
