#include "indicators/ATR.hpp"

#include <algorithm>
#include <unordered_map>

ATR::ATR (const std::size_t period) : _period{ period } {}

bool
ATR::ready ()
{
  return _n >= _period;
}

void
ATR::push (const Bar &bar)
{
  if (_prev_close == -1.0)
    {
      _prev_close = bar.close ();
      return;
    }

  const double high{ bar.high () };
  const double low{ bar.low () };
  const double tr{ calc_tr (high, low, _prev_close) };
  if (!ready ())
    {
      ++_n;
    }

  _atr_value = (_atr_value * (_n - 1) + tr) / _n;
  _prev_close = bar.close ();
}

std::unordered_map<std::string, double>
ATR::snapshot ()
{
  if (!ready ())
    {
      throw std::runtime_error ("ATR::snapshot(): no valid ATR data");
    }

  return { { "atr", _atr_value } };
}

double
ATR::calc_tr (const double high, const double low, const double prev_close)
{
  return std::max ({ std::abs (high - low), std::abs (high - prev_close),
                     std::abs (low - prev_close) });
}
