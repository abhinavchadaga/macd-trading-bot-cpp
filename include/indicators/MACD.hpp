#pragma once

#include "EMA.hpp"
#include "Indicator.hpp"
#include "ScalarEMA.hpp"

#include <string>
#include <unordered_map>

class MACD final : Indicator
{
public:
  explicit MACD (std::size_t fast_period = 12, std::size_t slow_period = 26,
                 std::size_t signal_period = 9);

  bool ready () override;

  void push (const Bar &) override;

  std::unordered_map<std::string, double> snapshot () override;

private:
  EMA _fast_ema;
  EMA _slow_ema;
  ScalarEMA _signal_ema;

  double _macd_value{};
};
