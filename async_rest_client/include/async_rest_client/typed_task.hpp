#pragma once

#include "base_task.hpp"
#include "concepts.hpp"

#include <boost/beast.hpp>
#include <boost/url.hpp>
#include <future>
#include <string_view>

namespace async_rest_client
{

namespace beast = boost::beast;
namespace http  = beast::http;

template <typename ReqBody, typename ResBody>
  requires SupportedRequestBody<ReqBody> && SupportedResponseBody<ResBody>
class typed_task final : public base_task
{
public:

  typed_task(
    std::string_view             url,
    http::verb                   verb,
    http::fields                 headers,
    typename ReqBody::value_type request_payload);

  net::awaitable<bool> run(http_connection_context &ctx) override;
  net::awaitable<bool> run(https_connection_context &ctx) override;
  const boost::url    &endpoint() const override;

  std::shared_future<http::response<ResBody>>
  get_shared_future()
  {
    return _promise.get_future().share();
  }

private:

  template <SupportedStreamType Stream>
  net::awaitable<bool> run_impl(Stream &stream, beast::flat_buffer &buffer);

  std::promise<http::response<ResBody>> _promise;
  boost::url                            _endpoint;
  http::verb                            _verb;
  http::fields                          _headers;
  typename ReqBody::value_type          _request_payload;
};

} // namespace async_rest_client
