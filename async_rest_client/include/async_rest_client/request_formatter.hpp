#pragma once

#include <boost/beast/http.hpp>
#include <fmt/format.h>
#include <string_view>

template <>
struct fmt::formatter<
  boost::beast::http::request<boost::beast::http::string_body>>
{
  constexpr auto
  parse(fmt::format_parse_context &ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(
    const boost::beast::http::request<boost::beast::http::string_body> &req,
    FormatContext &ctx) const
  {
    // Pre-allocate buffer for efficiency
    fmt::memory_buffer buf;

    // First line: METHOD /path?query HTTP/version [key headers]
    fmt::format_to(
      std::back_inserter(buf),
      "{} {} HTTP/{}.{} [",
      req.method_string(),
      req.target(), // includes query parameters
      req.version() / 10,
      req.version() % 10);

    // Add selected headers in compact format
    bool first_header = true;
    for (const auto &field : req)
      {
        const auto name = field.name_string();
        // Only include important headers
        if (
          name == "Host" || name == "host" || name == "Content-Type"
          || name == "content-type" || name == "Content-Length"
          || name == "content-length" || name == "Authorization"
          || name == "authorization")
          {
            if (!first_header)
              {
                fmt::format_to(std::back_inserter(buf), ", ");
              }
            // Mask authorization header
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
    fmt::format_to(std::back_inserter(buf), "]\n");

    // Second line: Body
    fmt::format_to(std::back_inserter(buf), "Body: {}", req.body());

    return fmt::format_to(
      ctx.out(),
      "{}",
      std::string_view(buf.data(), buf.size()));
  }
};

template <>
struct fmt::formatter<
  boost::beast::http::request<boost::beast::http::empty_body>>
{
  constexpr auto
  parse(fmt::format_parse_context &ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(
    const boost::beast::http::request<boost::beast::http::empty_body> &req,
    FormatContext &ctx) const
  {
    // Pre-allocate buffer for efficiency
    fmt::memory_buffer buf;

    // First line: METHOD /path?query HTTP/version [key headers]
    fmt::format_to(
      std::back_inserter(buf),
      "{} {} HTTP/{}.{} [",
      req.method_string(),
      req.target(), // includes query parameters
      req.version() / 10,
      req.version() % 10);

    // Add selected headers in compact format
    bool first_header = true;
    for (const auto &field : req)
      {
        const auto name = field.name_string();
        // Only include important headers
        if (
          name == "Host" || name == "host" || name == "Content-Type"
          || name == "content-type" || name == "Content-Length"
          || name == "content-length" || name == "Authorization"
          || name == "authorization")
          {
            if (!first_header)
              {
                fmt::format_to(std::back_inserter(buf), ", ");
              }
            // Mask authorization header
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
    fmt::format_to(std::back_inserter(buf), "]");

    // No body line for empty_body

    return fmt::format_to(
      ctx.out(),
      "{}",
      std::string_view(buf.data(), buf.size()));
  }
};
