#pragma once
#include "Bar.hpp"
#include "IndicatorConfig.hpp"
#include "IndicatorRegistry.hpp"
#include "LoggingUtils.hpp"
#include "indicators/ohlcv/OHLCVIndicator.hpp"

#include <ranges>

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
class indicator_engine
{
public:

  using bar_type      = bar<Count, TimeUnit>;
  using registry_type = indicator_registry<IndicatorInterface>;
  using snapshots
    = std::unordered_map<std::string, typename IndicatorInterface::snapshot>;
  using indicator_container
    = std::unordered_map<std::string, std::unique_ptr<IndicatorInterface>>;

  explicit indicator_engine(const std::vector<indicator_config> &configs);

  void on_bar(const bar_type &bar);

  [[nodiscard]]
  bool is_ready() const;

  const snapshots &read();

private:

  indicator_container _indicators {};
  snapshots          _snapshots {};
};

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
indicator_engine<Count, TimeUnit, IndicatorInterface>::indicator_engine(
  const std::vector<indicator_config> &configs)
{
  configure_logging();
  CLASS_LOGGER(indicator_engine);

  for (const auto &config : configs)
    {
      auto [name_sv, indicator] { registry_type::create(config) };
      const std::string name { name_sv };
      LOG_INFO(indicator_engine, indicator_engine)
        << "registered " + name << " indicator!";
      _indicators[name] = std::move(indicator);
      _snapshots[name]  = std::move(typename IndicatorInterface::snapshot {});
    }
}

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
void
indicator_engine<Count, TimeUnit, IndicatorInterface>::on_bar(
  const bar_type &bar)
{
  for (const auto &indicator_ptr : _indicators | std::views::values)
    indicator_ptr->write(bar.ohlcv());
}

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
bool
indicator_engine<Count, TimeUnit, IndicatorInterface>::is_ready() const
{
  return std::ranges::all_of(
    _indicators.begin(),
    _indicators.end(),
    [](const auto &p) {
      return p.second->is_ready();
    });
}

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
const typename indicator_engine<Count, TimeUnit, IndicatorInterface>::
  snapshots &
  indicator_engine<Count, TimeUnit, IndicatorInterface>::read()
{
  if (!is_ready())
    {
      throw std::runtime_error {
        "Indicators in indicator_engine are not all ready"
      };
    }

  for (const auto &[name, indicator_ptr] : _indicators)
    {
      _snapshots[name] = std::move(indicator_ptr->read());
    }

  return _snapshots;
}

// Convenient type aliases for OHLCV indicators
using ohlcv_indicator_registry = indicator_registry<ohlcv_indicator>;

template <std::size_t Count, ChronoDuration TimeUnit>
using ohlcv_indicator_engine = indicator_engine<Count, TimeUnit, ohlcv_indicator>;

using default_indicator_engine = ohlcv_indicator_engine<5, std::chrono::minutes>;
