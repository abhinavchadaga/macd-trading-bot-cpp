#pragma once

#include "IndicatorConfig.hpp"
#include "OHLCVIndicator.hpp"

#include <cstddef>
#include <string_view>

class EMA final : public OHLCVIndicator {
 public:
  static constexpr std::size_t SMOOTHING_FACTOR{2};

  static constexpr std::string_view name{"EMA"};

  explicit EMA(std::size_t period);
  explicit EMA(const IndicatorConfig& config);

  //
  // Indicator methods

  [[nodiscard]] bool is_ready() const override;

  [[nodiscard]] Snapshot read() const override;

  void write(const OHLCV& ohlcv) override;

  //
  // EMA methods

  void write(double close);

  [[nodiscard]] std::size_t period() const;

 private:
  double _value{0.0};
  std::size_t _n{};
  double _alpha{};
  std::size_t _period{};
};
