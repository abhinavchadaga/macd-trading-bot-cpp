#pragma once

#include "Bar.hpp"
#include "IndicatorEngine.hpp"
#include "LoggingUtils.hpp"

#include <boost/signals2.hpp>
#include <optional>

enum class Signal
{
  BUY,
  SELL,
  NO_ACTION
};

enum class PositionState
{
  CASH,
  LONG
};

class Strategy
{
public:

  using Snapshots = DefaultIndicatorEngine::Snapshots;
  using signal_t  = boost::signals2::signal<void(Signal, const Bar5min &)>;

  struct Config
  {
    double atr_threshold = 0.02;    // ATR / close price threshold (2%)
    size_t ema_period    = 200;     // EMA period for trend filter
  };

  explicit Strategy(const Config &config = {});

  void on_bar(const Bar5min &bar);
  void on_indicators(const Snapshots &snapshots);

  boost::signals2::connection subscribe(const signal_t::slot_type &handler);

  [[nodiscard]]
  PositionState position_state() const;

  void set_position_state(PositionState state);

private:

  void evaluate_strategy();

  Signal evaluate_entry_conditions(
    const Bar5min   &bar,
    const Snapshots &snapshots) const;

  Signal evaluate_exit_conditions(
    const Bar5min   &bar,
    const Snapshots &snapshots) const;

  bool is_macd_bullish_crossover(const Snapshots &snapshots) const;
  bool is_macd_bearish_crossover(const Snapshots &snapshots) const;

  void update_previous_values(const Snapshots &snapshots);

  Config                         _config;
  PositionState                  _position_state { PositionState::CASH };
  signal_t                       _signal;
  std::optional<Bar5min>         _current_bar;
  std::optional<Snapshots>       _current_snapshots;
  std::optional<double>          _prev_macd_line;
  std::optional<double>          _prev_signal_line;
  std::optional<double>          _prev_ema;
};