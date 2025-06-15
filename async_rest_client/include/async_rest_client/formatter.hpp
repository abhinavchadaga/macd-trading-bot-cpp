#pragma once

#include <boost/beast/http.hpp>
#include <fmt/format.h>
#include <string_view>

namespace http = boost::beast::http;

namespace
{

template <typename Request>
void
format_request_line(fmt::memory_buffer &buf, const Request &req)
{
  fmt::format_to(
    std::back_inserter(buf),
    "{} {} HTTP/{}.{} [",
    req.method_string(),
    req.target(),
    req.version() / 10,
    req.version() % 10);
}

template <typename Request>
void
format_request_headers(fmt::memory_buffer &buf, const Request &req)
{
  bool first_header = true;
  for (const auto &field : req)
    {
      if (const auto name { field.name_string() };
          name == "Host" || name == "host" || name == "Content-Type"
          || name == "content-type" || name == "Content-Length"
          || name == "content-length" || name == "Authorization"
          || name == "authorization")
        {
          if (!first_header)
            {
              fmt::format_to(std::back_inserter(buf), ", ");
            }
          if (name == "Authorization" || name == "authorization")
            {
              fmt::format_to(std::back_inserter(buf), "{}: ***", name);
            }
          else
            {
              fmt::format_to(
                std::back_inserter(buf),
                "{}: {}",
                name,
                field.value());
            }
          first_header = false;
        }
    }
}

} // anonymous namespace

template <>
struct fmt::formatter<http::request<http::string_body>>
{
  static constexpr auto
  parse(const format_parse_context &ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(const http::request<http::string_body> &req, FormatContext &ctx) const
  {
    memory_buffer buf {};

    format_request_line(buf, req);
    format_request_headers(buf, req);
    fmt::format_to(std::back_inserter(buf), "]\n");
    fmt::format_to(std::back_inserter(buf), "Body: {}", req.body());

    return fmt::format_to(
      ctx.out(),
      "{}",
      std::string_view(buf.data(), buf.size()));
  }
};

template <>
struct fmt::formatter<http::request<http::empty_body>>
{
  static constexpr auto
  parse(const format_parse_context &ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(const http::request<http::empty_body> &req, FormatContext &ctx) const
  {
    memory_buffer buf {};

    format_request_line(buf, req);
    format_request_headers(buf, req);
    fmt::format_to(std::back_inserter(buf), "]");

    return fmt::format_to(
      ctx.out(),
      "{}",
      std::string_view(buf.data(), buf.size()));
  }
};

template <>
struct fmt::formatter<http::response<http::string_body>>
{
  static constexpr auto
  parse(const format_parse_context &ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(const http::response<http::string_body> &res, FormatContext &ctx)
    const
  {
    memory_buffer buf {};

    fmt::format_to(
      std::back_inserter(buf),
      "HTTP/{}.{} {} {} [",
      res.version() / 10,
      res.version() % 10,
      res.result_int(),
      res.reason());

    bool first_header = true;
    for (const auto &field : res)
      {
        if (const auto name { field.name_string() };
            name == "Content-Type" || name == "content-type"
            || name == "Content-Length" || name == "content-length"
            || name == "Server" || name == "server" || name == "Set-Cookie"
            || name == "set-cookie")
          {
            if (!first_header)
              {
                fmt::format_to(std::back_inserter(buf), ", ");
              }
            if (name == "Set-Cookie" || name == "set-cookie")
              {
                fmt::format_to(std::back_inserter(buf), "{}: ***", name);
              }
            else
              {
                fmt::format_to(
                  std::back_inserter(buf),
                  "{}: {}",
                  name,
                  field.value());
              }
            first_header = false;
          }
      }
    fmt::format_to(std::back_inserter(buf), "]\n");

    fmt::format_to(std::back_inserter(buf), "Body: {}", res.body());

    return fmt::format_to(
      ctx.out(),
      "{}",
      std::string_view(buf.data(), buf.size()));
  }
};
