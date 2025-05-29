#include "indicators/MACD.hpp"

#include <unordered_map>

MACD::MACD (const std::size_t fast_period, const std::size_t slow_period,
            const std::size_t signal_period)
    : _fast_ema{ fast_period }, _slow_ema{ slow_period },
      _signal_ema{ signal_period }
{
}

bool
MACD::ready ()
{
  return _fast_ema.ready () && _slow_ema.ready () && _signal_ema.ready ();
}

void
MACD::push (const Bar &bar)
{
  _fast_ema.push (bar);
  _slow_ema.push (bar);

  if (_fast_ema.ready () && _slow_ema.ready ())
    {
      const auto &[f_key, fast_ema] = *_fast_ema.snapshot ().begin ();
      const auto &[s_key, slow_ema] = *_slow_ema.snapshot ().begin ();

      _macd_value = fast_ema - slow_ema;
      _signal_ema.push (_macd_value);
    }
}

std::unordered_map<std::string, double>
MACD::snapshot ()
{
  if (!ready ())
    throw std::runtime_error ("MACD::snapshot(): no data available");

  return { { "macd", _macd_value },
           { "signal", _signal_ema.value () },
           { "histogram", _macd_value - _signal_ema.value () } };
}
