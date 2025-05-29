#include "indicators/EMACalculator.hpp"

void
EMACalculator::update (const double new_value, double &ema_value,
                       std::size_t &n_periods, const std::size_t window)
{
  if (!ready (n_periods, window))
    {
      ++n_periods;
      ema_value = (ema_value + new_value) / static_cast<double> (n_periods);
    }
  else
    {
      ema_value
          = new_value
                * (SMOOTHING_FACTOR / (1 + static_cast<double> (n_periods)))
            + ema_value
                  * (1
                     - SMOOTHING_FACTOR
                           / (1 + static_cast<double> (n_periods)));
      ++n_periods;
    }
}

bool
EMACalculator::ready (const std::size_t n_periods, const std::size_t window)
{
  return n_periods >= window;
}
