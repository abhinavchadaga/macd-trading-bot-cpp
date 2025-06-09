#pragma once

#include <string>
#include <string_view>

class url_components
{
public:

  static url_components parse(std::string_view url);

  const std::string &hostname() const noexcept;
  const std::string &port() const noexcept;
  const std::string &target() const noexcept;

private:

  url_components(std::string hostname, std::string port, std::string target);

  std::string _hostname;
  std::string _port;
  std::string _target;
};
