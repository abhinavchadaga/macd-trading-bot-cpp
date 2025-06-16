#pragma once

#include "IndicatorConfig.hpp"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

template<typename IndicatorInterface>
class IndicatorRegistry
{
public:
    using FactoryFn =
        std::function<std::pair<std::string_view, std::unique_ptr<IndicatorInterface>>(const IndicatorConfig&)>;

    static IndicatorRegistry& instance();

    static void register_indicator(std::string_view name, FactoryFn factory);

    [[nodiscard]]
    static std::pair<std::string_view, std::unique_ptr<IndicatorInterface>> create(const IndicatorConfig& config);

private:
    std::unordered_map<std::string, FactoryFn> _factories{};
};

template<typename IndicatorInterface>
IndicatorRegistry<IndicatorInterface>& IndicatorRegistry<IndicatorInterface>::instance()
{
    static IndicatorRegistry instance{};
    return instance;
}

template<typename IndicatorInterface>
void IndicatorRegistry<IndicatorInterface>::register_indicator(const std::string_view name, FactoryFn factory)
{
    instance()._factories[std::string{name}] = std::move(factory);
}

template<typename IndicatorInterface>
std::pair<std::string_view, std::unique_ptr<IndicatorInterface>>
    IndicatorRegistry<IndicatorInterface>::create(const IndicatorConfig& config)
{
    const auto it{instance()._factories.find(std::string{config.name})};
    if (it == instance()._factories.end())
    {
        throw std::runtime_error{"Unknown indicator: " + std::string{config.name}};
    }
    const auto& [name, factory] = *it;
    return factory(config);
}
