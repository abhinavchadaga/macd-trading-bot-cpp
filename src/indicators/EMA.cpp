#include "indicators/EMA.hpp"
#include "indicators/EMACalculator.hpp"

#include <cstddef>
#include <format>
#include <unordered_map>

EMA::EMA (const std::size_t window) : _window{ window } {}

bool
EMA::ready ()
{
  return EMACalculator::ready (_n_periods, _window);
}

void
EMA::push (const Bar &bar)
{
  EMACalculator::update (bar.close (), _value, _n_periods, _window);
}

std::unordered_map<std::string, double>
EMA::snapshot ()
{
  if (!ready ())
    throw std::runtime_error ("EMA::snapshot(): not ready");

  std::string key{ std::format ("ema_{}", _window) };
  return { { key, _value } };
}
