#include "Strategy.hpp"

#include <stdexcept>

Strategy::Strategy(const Config &config)
  : _config { config }
{
  configure_logging();
  CLASS_LOGGER(Strategy);
}

void
Strategy::on_bar(const Bar5min &bar)
{
  _current_bar = bar;
  
  // If we have both bar and indicators, evaluate strategy
  if (_current_snapshots.has_value())
    {
      evaluate_strategy();
    }
}

void
Strategy::on_indicators(const Snapshots &snapshots)
{
  _current_snapshots = snapshots;
  
  // If we have both bar and indicators, evaluate strategy
  if (_current_bar.has_value())
    {
      evaluate_strategy();
    }
}

void
Strategy::evaluate_strategy()
{
  CLASS_LOGGER(Strategy);
  
  if (!_current_bar.has_value() || !_current_snapshots.has_value())
    {
      return;
    }

  const auto &bar       = _current_bar.value();
  const auto &snapshots = _current_snapshots.value();
  
  Signal signal = Signal::NO_ACTION;
  
  if (_position_state == PositionState::CASH)
    {
      signal = evaluate_entry_conditions(bar, snapshots);
    }
  else if (_position_state == PositionState::LONG)
    {
      signal = evaluate_exit_conditions(bar, snapshots);
    }
  
  if (signal != Signal::NO_ACTION)
    {
      LOG_INFO(Strategy, evaluate_strategy)
        << "Generated signal: " 
        << (signal == Signal::BUY ? "BUY" : "SELL")
        << " at price: " << bar.close();
      
      _signal(signal, bar);
      
      // Update position state based on signal
      if (signal == Signal::BUY)
        {
          _position_state = PositionState::LONG;
        }
      else if (signal == Signal::SELL)
        {
          _position_state = PositionState::CASH;
        }
    }
  
  // Update previous values for crossover detection
  update_previous_values(snapshots);
}

Signal
Strategy::evaluate_entry_conditions(
  const Bar5min   &bar,
  const Snapshots &snapshots) const
{
  CLASS_LOGGER(Strategy);
  
  // Validate required indicators are present
  if (!snapshots.contains("EMA") || !snapshots.contains("MACD") 
      || !snapshots.contains("ATR"))
    {
      LOG_ERROR(Strategy, evaluate_entry_conditions)
        << "Missing required indicators in snapshots";
      return Signal::NO_ACTION;
    }

  const auto &ema_snapshot  = snapshots.at("EMA");
  const auto &macd_snapshot = snapshots.at("MACD");
  const auto &atr_snapshot  = snapshots.at("ATR");
  
  // Extract indicator values
  const double close_price  = bar.close();
  const double ema_value    = ema_snapshot.at("ema");
  const double macd_line    = macd_snapshot.at("macd");
  const double signal_line  = macd_snapshot.at("signal");
  const double atr_value    = atr_snapshot.at("atr");
  
  // Entry conditions from README:
  // 1. close price > EMA_200
  const bool above_ema = close_price > ema_value;
  
  // 2. MACD line cross above the MACD signal line
  const bool macd_bullish_cross = is_macd_bullish_crossover(snapshots);
  
  // 3. MACD line is below zero  
  const bool macd_below_zero = macd_line < 0.0;
  
  // 4. ATR / close price < ATR threshold
  const double atr_ratio = atr_value / close_price;
  const bool low_volatility = atr_ratio < _config.atr_threshold;
  
  LOG_INFO(Strategy, evaluate_entry_conditions)
    << "Entry conditions - Above EMA: " << above_ema
    << ", MACD Bullish Cross: " << macd_bullish_cross  
    << ", MACD Below Zero: " << macd_below_zero
    << ", Low Volatility (ATR ratio " << atr_ratio << " < " 
    << _config.atr_threshold << "): " << low_volatility;
  
  // All four conditions must be met
  if (above_ema && macd_bullish_cross && macd_below_zero && low_volatility)
    {
      LOG_INFO(Strategy, evaluate_entry_conditions)
        << "All entry conditions met - generating BUY signal";
      return Signal::BUY;
    }
  
  return Signal::NO_ACTION;
}

Signal
Strategy::evaluate_exit_conditions(
  const Bar5min   &bar,
  const Snapshots &snapshots) const
{
  CLASS_LOGGER(Strategy);
  
  // Validate required indicators are present
  if (!snapshots.contains("EMA") || !snapshots.contains("MACD"))
    {
      LOG_ERROR(Strategy, evaluate_exit_conditions)
        << "Missing required indicators in snapshots";
      return Signal::NO_ACTION;
    }

  const auto &ema_snapshot = snapshots.at("EMA");
  
  // Extract indicator values  
  const double close_price = bar.close();
  const double ema_value   = ema_snapshot.at("ema");
  
  // Exit conditions from README:
  // 1. close price < EMA_200
  const bool below_ema = close_price < ema_value;
  
  // 2. MACD line crosses below the MACD signal line
  const bool macd_bearish_cross = is_macd_bearish_crossover(snapshots);
  
  LOG_INFO(Strategy, evaluate_exit_conditions)
    << "Exit conditions - Below EMA: " << below_ema
    << ", MACD Bearish Cross: " << macd_bearish_cross;
  
  // Either condition triggers exit
  if (below_ema || macd_bearish_cross)
    {
      LOG_INFO(Strategy, evaluate_exit_conditions)
        << "Exit condition met - generating SELL signal";
      return Signal::SELL;
    }
  
  return Signal::NO_ACTION;
}

bool
Strategy::is_macd_bullish_crossover(const Snapshots &snapshots) const
{
  if (!_prev_macd_line.has_value() || !_prev_signal_line.has_value())
    {
      return false; // Can't detect crossover without previous values
    }
  
  const auto &macd_snapshot = snapshots.at("MACD");
  const double curr_macd    = macd_snapshot.at("macd");
  const double curr_signal  = macd_snapshot.at("signal");
  
  // Bullish crossover: MACD was below signal, now above
  const bool was_below = _prev_macd_line.value() <= _prev_signal_line.value();
  const bool now_above = curr_macd > curr_signal;
  
  return was_below && now_above;
}

bool
Strategy::is_macd_bearish_crossover(const Snapshots &snapshots) const
{
  if (!_prev_macd_line.has_value() || !_prev_signal_line.has_value())
    {
      return false; // Can't detect crossover without previous values  
    }
  
  const auto &macd_snapshot = snapshots.at("MACD");
  const double curr_macd    = macd_snapshot.at("macd");
  const double curr_signal  = macd_snapshot.at("signal");
  
  // Bearish crossover: MACD was above signal, now below
  const bool was_above = _prev_macd_line.value() >= _prev_signal_line.value();
  const bool now_below = curr_macd < curr_signal;
  
  return was_above && now_below;
}

void
Strategy::update_previous_values(const Snapshots &snapshots)
{
  if (snapshots.contains("MACD"))
    {
      const auto &macd_snapshot = snapshots.at("MACD");
      _prev_macd_line    = macd_snapshot.at("macd");
      _prev_signal_line  = macd_snapshot.at("signal");
    }
  
  if (snapshots.contains("EMA"))
    {
      const auto &ema_snapshot = snapshots.at("EMA");
      _prev_ema = ema_snapshot.at("ema");
    }
}

boost::signals2::connection
Strategy::subscribe(const signal_t::slot_type &handler)
{
  return _signal.connect(handler);
}

PositionState
Strategy::position_state() const
{
  return _position_state;
}

void
Strategy::set_position_state(const PositionState state)
{
  _position_state = state;
}