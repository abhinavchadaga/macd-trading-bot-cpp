#pragma once

#include "IndicatorRegistry.hpp"

#include <memory>
#include <string_view>

template <typename T>
concept HasName = requires {
  {
    T::name
  } -> std::convertible_to<std::string_view>;
};

template <HasName DerivedIndicator, typename IndicatorInterface>
struct IndicatorRegistrar
{
  IndicatorRegistrar()
  {
    IndicatorRegistry<IndicatorInterface>::register_indicator(
      DerivedIndicator::name,
      [](const IndicatorConfig &config) {
        return std::make_pair(
          DerivedIndicator::name,
          std::make_unique<DerivedIndicator>(config));
      });
  }
};

#define REGISTER_INDICATOR(Indicator, IndicatorInterface)                     \
  static IndicatorRegistrar<Indicator, IndicatorInterface>                    \
    _registrar##Indicator;
