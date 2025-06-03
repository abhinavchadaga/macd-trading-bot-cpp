#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

struct indicator_config
{
  std::string_view                     name;
  std::unordered_map<std::string, int> params {};
};
