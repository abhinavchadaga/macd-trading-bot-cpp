#pragma once

#include "Bar.hpp"

#include <string>
#include <unordered_map>

class ohlcv_indicator
{
public:

  using snapshot = std::unordered_map<std::string, double>;

  virtual ~ohlcv_indicator() = default;

  [[nodiscard]]
  virtual bool is_ready() const
    = 0;

  [[nodiscard]]
  virtual snapshot read() const
    = 0;

  virtual void write(const ohlcv &ohlcv) = 0;
};
