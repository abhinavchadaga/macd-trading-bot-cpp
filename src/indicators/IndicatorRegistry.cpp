#include "IndicatorRegistry.hpp"

#include "IndicatorConfig.hpp"

IndicatorRegistry &
IndicatorRegistry::instance ()
{
  static IndicatorRegistry instance{};
  return instance;
}

void
IndicatorRegistry::register_indicator (const std::string_view name,
                                       FactoryFn factory)
{
  _factories[std::string{ name }] = std::move (factory);
}

std::pair<std::string_view, std::unique_ptr<Indicator> >
IndicatorRegistry::create (const IndicatorConfig &config) const
{
  const auto it{ _factories.find (std::string{ config.name }) };
  if (it == _factories.end ())
    {
      throw std::runtime_error{ "Unknown indicator: "
                                + std::string{ config.name } };
    }
  const auto &[name, factory] = *it;
  return factory (config);
}
