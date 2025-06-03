#pragma once

#include "IndicatorConfig.hpp"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

template <typename IndicatorInterface>
class indicator_registry
{
public:

  using factory_fn = std::function<
    std::pair<std::string_view, std::unique_ptr<IndicatorInterface>>(
      const indicator_config &)>;

  static indicator_registry &instance();

  static void register_indicator(std::string_view name, factory_fn factory);

  [[nodiscard]]
  static std::pair<std::string_view, std::unique_ptr<IndicatorInterface>>
  create(const indicator_config &config);

private:

  std::unordered_map<std::string, factory_fn> _factories {};
};

template <typename IndicatorInterface>
indicator_registry<IndicatorInterface> &
indicator_registry<IndicatorInterface>::instance()
{
  static indicator_registry instance {};
  return instance;
}

template <typename IndicatorInterface>
void
indicator_registry<IndicatorInterface>::register_indicator(
  const std::string_view name,
  factory_fn              factory)
{
  instance()._factories[std::string { name }] = std::move(factory);
}

template <typename IndicatorInterface>
std::pair<std::string_view, std::unique_ptr<IndicatorInterface>>
indicator_registry<IndicatorInterface>::create(const indicator_config &config)
{
  const auto it { instance()._factories.find(std::string { config.name }) };
  if (it == instance()._factories.end())
    {
      throw std::runtime_error { "Unknown indicator: "
                                 + std::string { config.name } };
    }
  const auto &[name, factory] = *it;
  return factory(config);
}
