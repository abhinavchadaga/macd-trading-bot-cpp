#pragma once

#include "indicators/Indicator.hpp"

#include <functional>
#include <unordered_map>

using FactoryFn
    = std::function<std::pair<std::string_view, std::unique_ptr<Indicator> > (
        const IndicatorConfig &)>;

class IndicatorRegistry
{
public:
  static IndicatorRegistry &instance ();

  void register_indicator (std::string_view name, FactoryFn factory);

  [[nodiscard]] std::pair<std::string_view, std::unique_ptr<Indicator> >
  create (const IndicatorConfig &config) const;

private:
  std::unordered_map<std::string, FactoryFn> _factories{};
};
