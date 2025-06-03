#pragma once

#include "IndicatorEngine.hpp"

#include <functional>
#include <unordered_map>

enum class TradeAction
{
  BUY,
  SELL,
  NO_ACTION
};

class MacdAtrStrategy
{
public:

  using Snapshots = std::unordered_map<std::string, std::unordered_map<std::string, double>>;
  using CallbackFunction = std::function<void(TradeAction, double)>;

  explicit MacdAtrStrategy(CallbackFunction callback);

  void on_indicator_update(
    const Snapshots &snapshots,
    double           current_price);

  [[nodiscard]]
  bool has_position() const;

  [[nodiscard]]
  double entry_price() const;

  [[nodiscard]]
  double stop_loss_price() const;

private:

  enum class PositionState
  {
    NO_POSITION,
    LONG_POSITION
  };

  struct IndicatorState
  {
    double macd_line {};
    double signal_line {};
    double ema_200 {};
    double atr {};
    bool   is_valid { false };
  };

  bool check_entry_conditions(const IndicatorState &current, double current_price) const;
  bool check_exit_conditions(const IndicatorState &current, double current_price) const;
  bool is_macd_crossover_above() const;
  bool is_macd_crossover_below() const;
  void update_state(const IndicatorState &new_state);
  IndicatorState extract_indicators(const Snapshots &snapshots) const;

  CallbackFunction _callback;
  PositionState    _position_state { PositionState::NO_POSITION };
  double           _entry_price {};
  double           _stop_loss_price {};
  IndicatorState   _previous_state {};
  IndicatorState   _current_state {};
};