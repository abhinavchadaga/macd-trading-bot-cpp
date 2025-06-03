#include "indicators/ohlcv/EMA.hpp"

#include "IndicatorRegistrar.hpp"

#include <stdexcept>

REGISTER_INDICATOR(EMA, OHLCVIndicator);

EMA::EMA(const std::size_t period)
  : _alpha { static_cast<double>(SMOOTHING_FACTOR)
             / (1 + static_cast<double>(period)) }
  , _period { period }
{
}

EMA::EMA(const IndicatorConfig &config)
  : EMA { [&config]() {
    if (const auto it = config.params.find("period");
        it != config.params.end())
      {
        return static_cast<std::size_t>(it->second);
      }
    throw std::runtime_error { "invalid config for EMA" };
  }() }
{
}

//
// Indicator methods

bool
EMA::is_ready() const
{
  return _n >= _period;
}

OHLCVIndicator::Snapshot
EMA::read() const
{
  if (!is_ready())
    {
      throw std::runtime_error("Not ready");
    }

  return {
    { "ema", _value }
  };
}

void
EMA::write(const OHLCV &ohlcv)
{
  write(ohlcv.close);
}

//
// EMA methods

void
EMA::write(const double close)
{
  if (is_ready())
    {
      _value = close * _alpha + _value * (1 - _alpha);
    }
  else
    {
      ++_n;
      _value = (_value * static_cast<double>(_n - 1) + close)
             / static_cast<double>(_n);
    }
}

std::size_t
EMA::period() const
{
  return _period;
}
