#pragma once
#include "Bar.hpp"
#include "IndicatorConfig.hpp"
#include "IndicatorRegistry.hpp"
#include "LoggingUtils.hpp"
#include "indicators/ohlcv/OHLCVIndicator.hpp"

#include <boost/signals2.hpp>
#include <ranges>

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
class IndicatorEngine
{
public:

  using BarType      = Bar<Count, TimeUnit>;
  using RegistryType = IndicatorRegistry<IndicatorInterface>;
  using Snapshots
    = std::unordered_map<std::string, typename IndicatorInterface::Snapshot>;
  using IndicatorContainer
    = std::unordered_map<std::string, std::unique_ptr<IndicatorInterface>>;
  using indicator_signal_t = boost::signals2::signal<void(const Snapshots &)>;

  explicit IndicatorEngine(const std::vector<IndicatorConfig> &configs);

  void on_bar(const BarType &bar);

  [[nodiscard]]
  bool is_ready() const;

  boost::signals2::connection
  subscribe(const typename indicator_signal_t::slot_type &handler)
  {
    return _indicator_signal.connect(handler);
  }

private:

  void update_snapshots();

  IndicatorContainer _indicators {};
  Snapshots          _snapshots {};
  indicator_signal_t _indicator_signal {};
};

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
IndicatorEngine<Count, TimeUnit, IndicatorInterface>::IndicatorEngine(
  const std::vector<IndicatorConfig> &configs)
{
  configure_logging();
  CLASS_LOGGER(IndicatorEngine);

  for (const auto &config : configs)
    {
      auto [name_sv, indicator] { RegistryType::create(config) };
      const std::string name { name_sv };
      LOG_INFO(IndicatorEngine, IndicatorEngine)
        << "registered " + name << " indicator!";
      _indicators[name] = std::move(indicator);
      _snapshots[name]  = std::move(typename IndicatorInterface::Snapshot {});
    }
}

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
void
IndicatorEngine<Count, TimeUnit, IndicatorInterface>::on_bar(
  const BarType &bar)
{
  for (const auto &indicator_ptr : _indicators | std::views::values)
    indicator_ptr->write(bar.ohlcv());

  if (is_ready())
    {
      update_snapshots();
      _indicator_signal(_snapshots);
    }
}

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
void
IndicatorEngine<Count, TimeUnit, IndicatorInterface>::update_snapshots()
{
  for (const auto &[name, indicator_ptr] : _indicators)
    {
      _snapshots[name] = std::move(indicator_ptr->read());
    }
}

template <
  std::size_t    Count,
  ChronoDuration TimeUnit,
  typename IndicatorInterface>
bool
IndicatorEngine<Count, TimeUnit, IndicatorInterface>::is_ready() const
{
  return std::ranges::all_of(
    _indicators.begin(),
    _indicators.end(),
    [](const auto &p) {
      return p.second->is_ready();
    });
}

// Convenient type aliases for OHLCV indicators
using OHLCVIndicatorRegistry = IndicatorRegistry<OHLCVIndicator>;

template <std::size_t Count, ChronoDuration TimeUnit>
using OHLCVIndicatorEngine = IndicatorEngine<Count, TimeUnit, OHLCVIndicator>;

using DefaultIndicatorEngine = OHLCVIndicatorEngine<5, std::chrono::minutes>;
