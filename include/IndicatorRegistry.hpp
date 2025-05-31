#pragma once

#include "indicators/Indicator.hpp"

#include <functional>
#include <unordered_map>

using FactoryFn =
    std::function<std::pair<std::string_view, std::unique_ptr<Indicator> >(
        const IndicatorConfig&)>;

class IndicatorRegistry {
 public:
  static IndicatorRegistry& instance();

  static void register_indicator(std::string_view name, FactoryFn factory);

  [[nodiscard]] static std::pair<std::string_view, std::unique_ptr<Indicator> >
  create(const IndicatorConfig& config);

 private:
  std::unordered_map<std::string, FactoryFn> _factories{};
};
