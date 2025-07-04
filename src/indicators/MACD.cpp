#include "indicators/ohlcv/MACD.hpp"

#include "IndicatorRegistrar.hpp"

REGISTER_INDICATOR(MACD, OHLCVIndicator);

namespace
{
std::size_t extract_period(const IndicatorConfig& config, const std::string_view key)
{
    const auto it = config.params.find(std::string{key});
    if (it == config.params.end())
    {
        throw std::runtime_error{"MACD config requires a " + std::string{key} + " key"};
    }
    return static_cast<std::size_t>(it->second);
}
} // namespace

MACD::MACD(const std::size_t fast_period, const std::size_t slow_period, const std::size_t signal_period)
    : _fast_ema{fast_period},
      _slow_ema{slow_period},
      _signal_ema{signal_period}
{
}

MACD::MACD(const IndicatorConfig& config)
    : MACD{
          extract_period(config, "fast_period"),
          extract_period(config, "slow_period"),
          extract_period(config, "signal_period")}
{
}

bool MACD::is_ready() const
{
    return _slow_ema.is_ready() && _fast_ema.is_ready() && _signal_ema.is_ready();
}

void MACD::write(const OHLCV& ohlcv)
{
    bool should_calc_signal{_fast_ema.is_ready() && _slow_ema.is_ready()};

    _fast_ema.write(ohlcv);
    _slow_ema.write(ohlcv);

    if (should_calc_signal)
    {
        const double fast_ema_value = _fast_ema.read().begin()->second;
        const double slow_ema_value = _slow_ema.read().begin()->second;
        _signal_ema.write(fast_ema_value - slow_ema_value);
    }
}

OHLCVIndicator::Snapshot MACD::read() const
{
    if (!is_ready())
    {
        throw std::runtime_error("MACD is not ready");
    }
    const double fast_ema_value = _fast_ema.read().begin()->second;
    const double slow_ema_value = _slow_ema.read().begin()->second;
    const double signal_line    = _signal_ema.read().begin()->second;

    const double macd_line = fast_ema_value - slow_ema_value;
    const double histogram = macd_line - signal_line;

    return {{"macd", macd_line}, {"signal", signal_line}, {"histogram", histogram}};
}
