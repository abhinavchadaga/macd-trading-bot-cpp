#include "indicators/ohlcv/ATR.hpp"

#include "IndicatorRegistrar.hpp"

#include <stdexcept>

REGISTER_INDICATOR(ATR, OHLCVIndicator)

ATR::ATR(const std::size_t period) : _period{period}
{
}

ATR::ATR(const IndicatorConfig& config)
    : ATR{[&config]()
          {
              if (const auto it = config.params.find("period"); it != config.params.end())
              {
                  return static_cast<std::size_t>(it->second);
              }
              throw std::runtime_error{"invalid config for ATR"};
          }()}
{
}

std::size_t ATR::period() const
{
    return _period;
}

bool ATR::is_ready() const
{
    return _n >= _period;
}

void ATR::write(const OHLCV& ohlcv)
{
    if (_prev_close == -1.0)
    {
        _prev_close = ohlcv.close;
        return;
    }

    const double high{ohlcv.high};
    const double low{ohlcv.low};
    const double tr{calc_tr(high, low, _prev_close)};
    if (!is_ready())
    {
        ++_n;
    }

    _val        = (_val * static_cast<double>(_n - 1) + tr) / static_cast<double>(_n);
    _prev_close = ohlcv.close;
}

OHLCVIndicator::Snapshot ATR::read() const
{
    if (!is_ready())
    {
        throw std::runtime_error("ATR::read(): no valid ATR data");
    }

    return {{"atr", _val}};
}

double ATR::calc_tr(const double high, const double low, const double prev_close)
{
    return std::max({std::abs(high - low), std::abs(high - prev_close), std::abs(low - prev_close)});
}
