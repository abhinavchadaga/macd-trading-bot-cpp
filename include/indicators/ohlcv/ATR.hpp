#pragma once

#include "IndicatorConfig.hpp"
#include "OHLCVIndicator.hpp"

#include <cstddef>
#include <string_view>

class atr final : public ohlcv_indicator
{
public:

  static constexpr std::string_view name { "ATR" };

  explicit atr(std::size_t period = 14);
  explicit atr(const indicator_config &config);

  //
  // Indicator methods

  [[nodiscard]]
  bool is_ready() const override;

  [[nodiscard]]
  snapshot read() const override;

  void write(const ohlcv &ohlcv) override;

  //
  // ATR methods

  [[nodiscard]]
  std::size_t period() const;

private:

  static double calc_tr(double high, double low, double prev_close);

  double      _val { 0.0 };
  double      _prev_close { -1.0 };
  std::size_t _n { 0 };
  std::size_t _period { 14 };
};
