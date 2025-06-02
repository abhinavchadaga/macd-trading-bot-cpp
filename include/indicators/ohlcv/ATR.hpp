#pragma once

#include "IndicatorConfig.hpp"
#include "OHLCVIndicator.hpp"

#include <cstddef>
#include <string_view>

class ATR final : public OHLCVIndicator {
 public:
  static constexpr std::string_view name{"ATR"};

  explicit ATR(std::size_t period = 14);
  explicit ATR(const IndicatorConfig& config);

  //
  // Indicator methods

  [[nodiscard]] bool is_ready() const override;

  [[nodiscard]] Snapshot read() const override;

  void write(const OHLCV& ohlcv) override;

  //
  // ATR methods

  [[nodiscard]] std::size_t period() const;

 private:
  static double calc_tr(double high, double low, double prev_close);

  double _val{0.0};
  double _prev_close{-1.0};
  std::size_t _n{0};
  std::size_t _period{14};
};
