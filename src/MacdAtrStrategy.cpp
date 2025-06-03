#include "MacdAtrStrategy.hpp"

#include <stdexcept>

MacdAtrStrategy::MacdAtrStrategy(CallbackFunction callback)
  : _callback { std::move(callback) }
{
  if (!_callback)
    {
      throw std::invalid_argument { "MacdAtrStrategy callback cannot be null" };
    }
}

void
MacdAtrStrategy::on_indicator_update(
  const Snapshots &snapshots,
  double           current_price)
{
  const auto new_state = extract_indicators(snapshots);
  
  if (!new_state.is_valid)
    {
      return;
    }

  update_state(new_state);

  if (_position_state == PositionState::NO_POSITION)
    {
      if (check_entry_conditions(_current_state, current_price))
        {
          _position_state = PositionState::LONG_POSITION;
          _entry_price = current_price;
          _stop_loss_price = current_price - (1.5 * _current_state.atr);
          _callback(TradeAction::BUY, current_price);
        }
    }
  else if (_position_state == PositionState::LONG_POSITION)
    {
      if (check_exit_conditions(_current_state, current_price))
        {
          _position_state = PositionState::NO_POSITION;
          _entry_price = 0.0;
          _stop_loss_price = 0.0;
          _callback(TradeAction::SELL, current_price);
        }
    }
}

bool
MacdAtrStrategy::has_position() const
{
  return _position_state == PositionState::LONG_POSITION;
}

double
MacdAtrStrategy::entry_price() const
{
  return _entry_price;
}

double
MacdAtrStrategy::stop_loss_price() const
{
  return _stop_loss_price;
}

bool
MacdAtrStrategy::check_entry_conditions(
  const IndicatorState &current,
  double                current_price) const
{
  return current_price > current.ema_200
      && is_macd_crossover_above()
      && current.macd_line < 0.0;
}

bool
MacdAtrStrategy::check_exit_conditions(
  const IndicatorState &current,
  double                current_price) const
{
  const bool stop_loss_hit = current_price <= _stop_loss_price;
  const bool ema_exit = current_price < current.ema_200;
  const bool macd_exit = is_macd_crossover_below();

  return stop_loss_hit || ema_exit || macd_exit;
}

bool
MacdAtrStrategy::is_macd_crossover_above() const
{
  if (!_previous_state.is_valid)
    {
      return false;
    }

  const bool prev_below = _previous_state.macd_line <= _previous_state.signal_line;
  const bool curr_above = _current_state.macd_line > _current_state.signal_line;

  return prev_below && curr_above;
}

bool
MacdAtrStrategy::is_macd_crossover_below() const
{
  if (!_previous_state.is_valid)
    {
      return false;
    }

  const bool prev_above = _previous_state.macd_line >= _previous_state.signal_line;
  const bool curr_below = _current_state.macd_line < _current_state.signal_line;

  return prev_above && curr_below;
}

void
MacdAtrStrategy::update_state(const IndicatorState &new_state)
{
  _previous_state = _current_state;
  _current_state = new_state;
}

MacdAtrStrategy::IndicatorState
MacdAtrStrategy::extract_indicators(const Snapshots &snapshots) const
{
  IndicatorState state {};

  const auto macd_it = snapshots.find("MACD");
  const auto ema_it = snapshots.find("EMA_200");
  const auto atr_it = snapshots.find("ATR");

  if (macd_it == snapshots.end() || ema_it == snapshots.end()
      || atr_it == snapshots.end())
    {
      return state;
    }

  const auto &macd_snapshot = macd_it->second;
  const auto &ema_snapshot = ema_it->second;
  const auto &atr_snapshot = atr_it->second;
  
  if (macd_snapshot.find("macd") == macd_snapshot.end()
      || macd_snapshot.find("signal") == macd_snapshot.end()
      || ema_snapshot.find("ema") == ema_snapshot.end()
      || atr_snapshot.find("atr") == atr_snapshot.end())
    {
      return state;
    }

  state.macd_line = macd_snapshot.at("macd");
  state.signal_line = macd_snapshot.at("signal");
  state.ema_200 = ema_snapshot.at("ema");
  state.atr = atr_snapshot.at("atr");
  state.is_valid = true;

  return state;
}