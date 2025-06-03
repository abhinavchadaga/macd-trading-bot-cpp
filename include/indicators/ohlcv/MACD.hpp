#pragma once

#include "EMA.hpp"
#include "IndicatorConfig.hpp"
#include "OHLCVIndicator.hpp"

#include <string_view>

class macd final : public ohlcv_indicator
{
public:

  static constexpr std::string_view name { "MACD" };

  explicit macd(
    std::size_t fast_period   = 12,
    std::size_t slow_period   = 26,
    std::size_t signal_period = 9);

  explicit macd(const indicator_config &config);

  //
  // Indicator methods

  [[nodiscard]]
  bool is_ready() const override;

  [[nodiscard]]
  snapshot read() const override;

  void write(const ohlcv &ohlcv) override;

private:

  ema _fast_ema;
  ema _slow_ema;
  ema _signal_ema;
};
