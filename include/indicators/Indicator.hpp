#pragma once

#include "Bar.hpp"
#include "IndicatorConfig.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

class Indicator
{
public:
  using Snapshot = std::unordered_map<std::string, double>;

  virtual ~Indicator () = default;

  [[nodiscard]] virtual bool is_ready () const = 0;

  [[nodiscard]] virtual Snapshot read () const = 0;

  virtual void write (const Bar &) = 0;
};
