#pragma once

#include <boost/beast/http.hpp>
#include <concepts>
#include <variant>

namespace beast = boost::beast;
namespace http  = beast::http;

template <typename... Bodies>
struct response_traits
{
  using variant_t = std::variant<http::response<Bodies>...>;
  template <typename U>
  static constexpr bool is_supported_v = (std::same_as<U, Bodies> || ...);
};

using response_body_traits = response_traits<http::string_body>;

template <typename ResponseBody>
concept SupportedResponseBody
  = response_body_traits::is_supported_v<ResponseBody>;

class response_container
{
public:

  using supported_body_types = response_body_traits::variant_t;

  template <SupportedResponseBody ResponseBody>
  void init();

  template <SupportedResponseBody ResponseBody>
  http::response<ResponseBody> &get();

private:

  supported_body_types _con {};
};

template <SupportedResponseBody ResponseBody>
inline void
response_container::init()
{
  _con.emplace<http::response<ResponseBody>>();
}

template <SupportedResponseBody ResponseBody>
inline http::response<ResponseBody> &
response_container::get()
{
  return std::get<ResponseBody>(_con);
}
