#pragma once

#include "Indicator.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>

class ATR final : Indicator
{
public:
  explicit ATR (std::size_t period = 14);

  bool ready () override;

  void push (const Bar &) override;

  std::unordered_map<std::string, double> snapshot () override;

private:
  static double calc_tr (double high, double low, double prev_close);

  std::size_t _period;
  double _atr_value{ 0.0 };
  double _prev_close{ -1.0 };
  std::size_t _n{ 0 };
};
