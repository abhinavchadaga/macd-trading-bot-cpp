#include "indicators/ScalarEMA.hpp"
#include "indicators/EMACalculator.hpp"

ScalarEMA::ScalarEMA (const std::size_t window) : _window{ window } {}

void
ScalarEMA::push (const double value)
{
  EMACalculator::update (value, _value, _n_periods, _window);
}

double
ScalarEMA::value () const
{
  return _value;
}

bool
ScalarEMA::ready () const
{
  return EMACalculator::ready (_n_periods, _window);
}
