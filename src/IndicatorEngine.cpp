#include "IndicatorEngine.hpp"

#include "IndicatorRegistry.hpp"
#include "LoggingUtils.hpp"

#include <ranges>

IndicatorEngine::IndicatorEngine(const std::vector<IndicatorConfig>& configs) {
  configure_logging();
  CLASS_LOGGER(IndicatorEngine);

  for (const auto& config : configs) {
    auto [name_sv, indicator]{IndicatorRegistry::create(config)};
    const std::string name{name_sv};
    LOG_INFO(IndicatorEngine, IndicatorEngine)
        << "registered " + name << " indicator!";
    _indicators[name] = std::move(indicator);
    _snapshots[name] = std::move(Indicator::Snapshot{});
  }
}

void IndicatorEngine::on_bar(const Bar& bar) {
  for (const auto& indicator_ptr : _indicators | std::views::values)
    indicator_ptr->write(bar);
}

bool IndicatorEngine::is_ready() const {
  return std::ranges::all_of(
      _indicators.begin(), _indicators.end(),
      [](const auto& p) { return p.second->is_ready(); });
}

const IndicatorEngine::Snapshots& IndicatorEngine::read() {
  if (!is_ready()) {
    throw std::runtime_error{"Indicators in IndicatorEngine are not all ready"};
  }

  for (const auto& [name, indicator_ptr] : _indicators) {
    _snapshots[name] = std::move(indicator_ptr->read());
  }

  return _snapshots;
}
