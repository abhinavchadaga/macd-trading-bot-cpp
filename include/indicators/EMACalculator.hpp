#pragma once

#include <cstddef>

class EMACalculator
{
public:
  constexpr static double SMOOTHING_FACTOR = 2;

  static void update (double new_value, double &ema_value,
                      std::size_t &n_periods, std::size_t window);

  static bool ready (std::size_t n_periods, std::size_t window);
};
