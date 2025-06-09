#include "async_rest_client/url_components.hpp"

#include <regex>
#include <stdexcept>

url_components::url_components(
  std::string hostname,
  std::string port,
  std::string target)
  : _hostname { std::move(hostname) }
  , _port { std::move(port) }
  , _target { std::move(target) }
{
}

url_components
url_components::parse(std::string_view url)
{
  static const std::regex url_regex {
    R"(^(https?)://([^:/]+)(?::(\d+))?(/.*)?$)"
  };

  std::string url_str { url };
  std::smatch matches;

  if (!std::regex_match(url_str, matches, url_regex))
    {
      throw std::invalid_argument { "Invalid URL format" };
    }

  std::string scheme { matches[1].str() };
  std::string hostname { matches[2].str() };
  std::string port;
  std::string target { matches[4].matched ? matches[4].str() : "/" };

  if (matches[3].matched)
    {
      port = matches[3].str();
    }
  else if (scheme == "http")
    {
      port = "80";
    }
  else if (scheme == "https")
    {
      port = "443";
    }
  else
    {
      throw std::invalid_argument {
        "Only http and https schemes are supported"
      };
    }

  return url_components { std::move(hostname),
                          std::move(port),
                          std::move(target) };
}

const std::string &
url_components::hostname() const noexcept
{
  return _hostname;
}

const std::string &
url_components::port() const noexcept
{
  return _port;
}

const std::string &
url_components::target() const noexcept
{
  return _target;
}
