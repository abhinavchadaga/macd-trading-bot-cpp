#include "Strategy.hpp"
#include "Bar.hpp"
#include "IndicatorEngine.hpp"
#include "LoggingUtils.hpp"

#include <gtest/gtest.h>
#include <atomic>

class StrategyTest : public ::testing::Test
{
protected:
  
  void SetUp() override
  {
    configure_logging();
    CLASS_LOGGER(StrategyTest);
    
    _strategy = std::make_unique<Strategy>();
    _signal_received = false;
    _last_signal = Signal::NO_ACTION;
    
    // Connect to strategy signals
    _strategy_connection = _strategy->subscribe(
      [this](Signal signal, const Bar5min &bar) {
        _signal_received = true;
        _last_signal = signal;
        _last_signal_bar = bar;
      });
  }

  void TearDown() override
  {
    _strategy_connection.disconnect();
  }

  // Helper function to create a test bar
  Bar5min create_test_bar(
    const std::string &symbol,
    double open, 
    double high, 
    double low, 
    double close,
    uint64_t volume = 1000) const
  {
    Bar1min::Timestamp timestamp{std::chrono::system_clock::now()};
    return Bar5min{symbol, open, high, low, close, volume, timestamp};
  }
  
  // Helper function to create indicator snapshots
  DefaultIndicatorEngine::Snapshots create_snapshots(
    double ema_value,
    double macd_line,
    double signal_line,
    double atr_value) const
  {
    return {
      {"EMA", {{"ema", ema_value}}},
      {"MACD", {{"macd", macd_line}, {"signal", signal_line}, {"histogram", macd_line - signal_line}}},
      {"ATR", {{"atr", atr_value}}}
    };
  }

  std::unique_ptr<Strategy> _strategy;
  boost::signals2::connection _strategy_connection;
  std::atomic<bool> _signal_received;
  Signal _last_signal;
  Bar5min _last_signal_bar{"", 0, 0, 0, 0, 0, {}};
};

TEST_F(StrategyTest, InitialStateIsCash)
{
  EXPECT_EQ(_strategy->position_state(), PositionState::CASH);
}

TEST_F(StrategyTest, NoSignalWithoutBothBarAndIndicators)
{
  // Send only bar
  auto bar = create_test_bar("PLTR", 100, 105, 99, 102);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load());
  
  // Send only indicators  
  auto snapshots = create_snapshots(100.0, -0.5, -1.0, 2.0);
  _strategy->on_indicators(snapshots);
  
  // Should not trigger signal yet since we haven't resent bar after indicators
  EXPECT_FALSE(_signal_received.load());
}

TEST_F(StrategyTest, BuySignalWhenAllEntryConditionsMet)
{
  // Set up conditions for BUY signal:
  // 1. close > EMA (102 > 100) ✓
  // 2. MACD bullish crossover (need previous values first)
  // 3. MACD below zero (-0.5 < 0) ✓  
  // 4. ATR/close < threshold (1.5/102 = 0.0147 < 0.02) ✓
  
  auto bar = create_test_bar("PLTR", 100, 105, 99, 102);
  
  // Send first set of indicators to establish previous values (no crossover yet)
  auto snapshots1 = create_snapshots(100.0, -1.0, -0.5, 1.5); // MACD below signal
  _strategy->on_indicators(snapshots1);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load()); // No crossover yet
  
  // Send second set with bullish crossover
  auto snapshots2 = create_snapshots(100.0, -0.5, -1.0, 1.5); // MACD above signal now
  _strategy->on_indicators(snapshots2);
  _strategy->on_bar(bar);
  
  EXPECT_TRUE(_signal_received.load());
  EXPECT_EQ(_last_signal, Signal::BUY);
  EXPECT_EQ(_strategy->position_state(), PositionState::LONG);
}

TEST_F(StrategyTest, NoMACDCrossoverWithoutPreviousValues)
{
  auto bar = create_test_bar("PLTR", 100, 105, 99, 102);
  
  // Perfect conditions except we have no previous MACD values for crossover
  auto snapshots = create_snapshots(100.0, -0.5, -1.0, 1.5);
  _strategy->on_indicators(snapshots);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load()); // No crossover possible
}

TEST_F(StrategyTest, NoEntryWhenConditionsMissing)
{
  auto bar = create_test_bar("PLTR", 100, 105, 99, 98); // close < EMA
  
  // Establish previous values
  auto snapshots1 = create_snapshots(100.0, -1.0, -0.5, 1.5);
  _strategy->on_indicators(snapshots1);
  _strategy->on_bar(bar);
  
  // Try crossover but close < EMA fails condition
  auto snapshots2 = create_snapshots(100.0, -0.5, -1.0, 1.5);
  _strategy->on_indicators(snapshots2);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load());
}

TEST_F(StrategyTest, NoEntryWhenMACDAboveZero)
{
  auto bar = create_test_bar("PLTR", 100, 105, 99, 102);
  
  // Establish previous values  
  auto snapshots1 = create_snapshots(100.0, -0.5, 0.5, 1.5);
  _strategy->on_indicators(snapshots1);
  _strategy->on_bar(bar);
  
  // MACD above zero fails condition
  auto snapshots2 = create_snapshots(100.0, 0.5, -0.5, 1.5);
  _strategy->on_indicators(snapshots2);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load());
}

TEST_F(StrategyTest, NoEntryWhenVolatilityTooHigh)
{
  auto bar = create_test_bar("PLTR", 100, 105, 99, 102);
  
  // Establish previous values
  auto snapshots1 = create_snapshots(100.0, -1.0, -0.5, 5.0);
  _strategy->on_indicators(snapshots1);
  _strategy->on_bar(bar);
  
  // High ATR ratio (5.0/102 = 0.049 > 0.02) fails condition
  auto snapshots2 = create_snapshots(100.0, -0.5, -1.0, 5.0);
  _strategy->on_indicators(snapshots2);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load());
}

TEST_F(StrategyTest, SellSignalWhenBelowEMA)
{
  // First, get into LONG position
  _strategy->set_position_state(PositionState::LONG);
  
  auto bar = create_test_bar("PLTR", 100, 105, 99, 98); // close < EMA
  auto snapshots = create_snapshots(100.0, -0.5, -1.0, 1.5);
  
  _strategy->on_indicators(snapshots);
  _strategy->on_bar(bar);
  
  EXPECT_TRUE(_signal_received.load());
  EXPECT_EQ(_last_signal, Signal::SELL);
  EXPECT_EQ(_strategy->position_state(), PositionState::CASH);
}

TEST_F(StrategyTest, SellSignalOnMACDBearishCrossover)
{
  // First, get into LONG position
  _strategy->set_position_state(PositionState::LONG);
  
  auto bar = create_test_bar("PLTR", 100, 105, 99, 102); // close > EMA (good)
  
  // Establish previous values with MACD above signal
  auto snapshots1 = create_snapshots(100.0, 0.5, -0.5, 1.5);
  _strategy->on_indicators(snapshots1);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load()); // No exit yet
  
  // MACD bearish crossover
  auto snapshots2 = create_snapshots(100.0, -0.5, 0.5, 1.5);
  _strategy->on_indicators(snapshots2);
  _strategy->on_bar(bar);
  
  EXPECT_TRUE(_signal_received.load());
  EXPECT_EQ(_last_signal, Signal::SELL);
  EXPECT_EQ(_strategy->position_state(), PositionState::CASH);
}

TEST_F(StrategyTest, NoExitWhenConditionsNotMet)
{
  // Get into LONG position
  _strategy->set_position_state(PositionState::LONG);
  
  auto bar = create_test_bar("PLTR", 100, 105, 99, 102); // close > EMA
  
  // No bearish crossover
  auto snapshots = create_snapshots(100.0, 0.5, -0.5, 1.5);
  _strategy->on_indicators(snapshots);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load());
  EXPECT_EQ(_strategy->position_state(), PositionState::LONG); // Still long
}

TEST_F(StrategyTest, IgnoresEntrySignalsWhenAlreadyLong)
{
  // Get into LONG position
  _strategy->set_position_state(PositionState::LONG);
  
  auto bar = create_test_bar("PLTR", 100, 105, 99, 102);
  
  // Perfect entry conditions
  auto snapshots1 = create_snapshots(100.0, -1.0, -0.5, 1.5);
  _strategy->on_indicators(snapshots1);
  _strategy->on_bar(bar);
  
  auto snapshots2 = create_snapshots(100.0, -0.5, -1.0, 1.5);
  _strategy->on_indicators(snapshots2);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load()); // Should not generate BUY when already LONG
}

TEST_F(StrategyTest, IgnoresExitSignalsWhenInCash)
{
  // Ensure we're in CASH state
  EXPECT_EQ(_strategy->position_state(), PositionState::CASH);
  
  auto bar = create_test_bar("PLTR", 100, 105, 99, 98); // close < EMA
  auto snapshots = create_snapshots(100.0, -0.5, -1.0, 1.5);
  
  _strategy->on_indicators(snapshots);
  _strategy->on_bar(bar);
  
  EXPECT_FALSE(_signal_received.load()); // Should not generate SELL when in CASH
}