#pragma once

#include "IndicatorRegistry.hpp"
#include "indicators/Indicator.hpp"

template <typename T> concept HasName = requires
{
  {
    T::name
  } -> std::convertible_to<std::string_view>;
};

template <HasName DerivedIndicator> struct IndicatorRegistrar
{
  IndicatorRegistrar ()
  {
    IndicatorRegistry::instance ().register_indicator (
        DerivedIndicator::name, [] (const IndicatorConfig &config) {
          return std::make_pair (DerivedIndicator::name,
                                 std::make_unique<DerivedIndicator> (config));
        });
  }
};

#define REGISTER_INDICATOR(Indicator)                                         \
  static IndicatorRegistrar<Indicator> _registrar##Indicator;
