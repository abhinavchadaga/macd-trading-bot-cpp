#include "indicators/ohlcv/ATR.hpp"

#include "IndicatorRegistrar.hpp"

#include <stdexcept>

REGISTER_INDICATOR(atr, ohlcv_indicator)

atr::atr(const std::size_t period)
  : _period { period }
{
}

atr::atr(const indicator_config &config)
  : atr { [&config]() {
    if (const auto it = config.params.find("period");
        it != config.params.end())
      {
        return static_cast<std::size_t>(it->second);
      }
    throw std::runtime_error { "invalid config for atr" };
  }() }
{
}

std::size_t
atr::period() const
{
  return _period;
}

bool
atr::is_ready() const
{
  return _n >= _period;
}

void
atr::write(const ohlcv &ohlcv)
{
  if (_prev_close == -1.0)
    {
      _prev_close = ohlcv.close;
      return;
    }

  const double high { ohlcv.high };
  const double low { ohlcv.low };
  const double tr { calc_tr(high, low, _prev_close) };
  if (!is_ready())
    {
      ++_n;
    }

  _val = (_val * static_cast<double>(_n - 1) + tr) / static_cast<double>(_n);
  _prev_close = ohlcv.close;
}

ohlcv_indicator::snapshot
atr::read() const
{
  if (!is_ready())
    {
      throw std::runtime_error("atr::read(): no valid atr data");
    }

  return {
    { "atr", _val }
  };
}

double
atr::calc_tr(const double high, const double low, const double prev_close)
{
  return std::max(
    { std::abs(high - low),
      std::abs(high - prev_close),
      std::abs(low - prev_close) });
}
