#pragma once

#include "IndicatorRegistry.hpp"

#include <memory>
#include <string_view>

template <typename T>
concept has_name = requires {
  {
    T::name
  } -> std::convertible_to<std::string_view>;
};

template <has_name DerivedIndicator, typename IndicatorInterface>
struct indicator_registrar
{
  indicator_registrar()
  {
    indicator_registry<IndicatorInterface>::register_indicator(
      DerivedIndicator::name,
      [](const indicator_config &config) {
        return std::make_pair(
          DerivedIndicator::name,
          std::make_unique<DerivedIndicator>(config));
      });
  }
};

#define REGISTER_INDICATOR(Indicator, IndicatorInterface)                     \
  static indicator_registrar<Indicator, IndicatorInterface>                    \
    _registrar##Indicator;
