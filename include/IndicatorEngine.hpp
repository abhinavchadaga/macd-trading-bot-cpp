#pragma once
#include "Bar.hpp"
#include "IndicatorConfig.hpp"
#include "indicators/Indicator.hpp"

class IndicatorEngine {
 public:
  using Snapshots = std::unordered_map<std::string, Indicator::Snapshot>;
  using IndicatorContainer =
      std::unordered_map<std::string, std::unique_ptr<Indicator> >;

  explicit IndicatorEngine(const std::vector<IndicatorConfig>& configs);

  void on_bar(const Bar& bar);

  [[nodiscard]] bool is_ready() const;

  const Snapshots& read();

 private:
  IndicatorContainer _indicators{};
  Snapshots _snapshots{};
};
