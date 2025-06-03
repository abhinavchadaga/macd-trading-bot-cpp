#pragma once

#include "IndicatorConfig.hpp"
#include "OHLCVIndicator.hpp"

#include <cstddef>
#include <string_view>

class ema final : public ohlcv_indicator
{
public:

  static constexpr std::size_t smoothing_factor { 2 };

  static constexpr std::string_view name { "EMA" };

  explicit ema(std::size_t period);
  explicit ema(const indicator_config &config);

  //
  // Indicator methods

  [[nodiscard]]
  bool is_ready() const override;

  [[nodiscard]]
  snapshot read() const override;

  void write(const ohlcv &ohlcv) override;

  //
  // EMA methods

  void write(double close);

  [[nodiscard]]
  std::size_t period() const;

private:

  double      _value { 0.0 };
  std::size_t _n {};
  double      _alpha {};
  std::size_t _period {};
};
