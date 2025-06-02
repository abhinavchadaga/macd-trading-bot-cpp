#pragma once

#include "Bar.hpp"

#include <string>
#include <unordered_map>

class OHLCVIndicator
{
public:

  using Snapshot = std::unordered_map<std::string, double>;

  virtual ~OHLCVIndicator() = default;

  [[nodiscard]]
  virtual bool is_ready() const
    = 0;

  [[nodiscard]]
  virtual Snapshot read() const
    = 0;

  virtual void write(const OHLCV &ohlcv) = 0;
};
