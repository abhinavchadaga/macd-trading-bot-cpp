#include <gtest/gtest.h>

#include "Bar.hpp"
#include "BarAggregator.hpp"
#include "IndicatorConfig.hpp"
#include "IndicatorEngine.hpp"
#include "Utils.hpp"
#include "indicators/ohlcv/ATR.hpp"
#include "indicators/ohlcv/EMA.hpp"
#include "indicators/ohlcv/MACD.hpp"

#include <filesystem>
#include <vector>

class IndicatorEngineIntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Set up indicator configurations
    IndicatorConfig ema_config{.name = "EMA", .params = {{"period", 20.0}}};

    IndicatorConfig atr_config{.name = "ATR", .params = {{"period", 14.0}}};

    IndicatorConfig macd_config{.name = "MACD",
                                .params = {{"fast_period", 12.0},
                                           {"slow_period", 26.0},
                                           {"signal_period", 9.0}}};

    indicator_configs = {ema_config, atr_config, macd_config};

    // Initialize components
    bar_aggregator = std::make_unique<BarAggregator<5, std::chrono::minutes>>();
    indicator_engine =
        std::make_unique<DefaultIndicatorEngine>(indicator_configs);

    // Connect aggregator to indicator engine
    aggregator_connection = bar_aggregator->connect_aggregated_bar_handler(
        [this](const Bar5min& aggregated_bar) {
          this->on_aggregated_bar(aggregated_bar);
        });
  }

  void TearDown() override { aggregator_connection.disconnect(); }

  void on_aggregated_bar(const Bar5min& bar) {
    indicator_engine->on_bar(bar);
    aggregated_bar_count++;

    // Track when indicators become ready
    if (indicator_engine->is_ready() && !indicators_ready) {
      indicators_ready = true;
      ready_after_bars = aggregated_bar_count;
    }

    // Collect snapshots when indicators are ready
    if (indicators_ready) {
      auto snapshots = indicator_engine->read();
      snapshot_history.push_back(snapshots);
    }
  }

  std::vector<IndicatorConfig> indicator_configs;
  std::unique_ptr<BarAggregator<5, std::chrono::minutes>> bar_aggregator;
  std::unique_ptr<DefaultIndicatorEngine> indicator_engine;
  boost::signals2::connection aggregator_connection;

  // Test tracking variables
  int aggregated_bar_count{0};
  bool indicators_ready{false};
  int ready_after_bars{0};
  std::vector<DefaultIndicatorEngine::Snapshots> snapshot_history;
};

TEST_F(IndicatorEngineIntegrationTest,
       ProcessesPLTRDataAndProducesSteadyIndicatorStream) {
  // Load PLTR CSV data
  const std::string csv_path =
      std::filesystem::current_path() / "test-utils" /
      "PLTR_2025-05-19_2025-05-23_1min_market_hours.csv";

  ASSERT_TRUE(std::filesystem::exists(csv_path))
      << "PLTR CSV file not found at: " << csv_path;

  auto input_bars = createBarsFromCSV(csv_path);
  ASSERT_GT(input_bars.size(), 0) << "No bars loaded from CSV";

  // Process bars through the pipeline, handling gaps gracefully
  int processed_bars = 0;
  int skipped_bars = 0;

  for (const auto& bar : input_bars) {
    try {
      bar_aggregator->on_bar(bar);
      processed_bars++;
    } catch (const std::runtime_error& e) {
      // Handle gaps in data (weekends, holidays, non-consecutive bars)
      if (std::string(e.what()).find(
              "timestamp does not match expected sequence") !=
          std::string::npos) {
        skipped_bars++;
        // Reset aggregator for new sequence
        bar_aggregator =
            std::make_unique<BarAggregator<5, std::chrono::minutes>>();
        aggregator_connection = bar_aggregator->connect_aggregated_bar_handler(
            [this](const Bar5min& aggregated_bar) {
              this->on_aggregated_bar(aggregated_bar);
            });
        // Try this bar again with fresh aggregator
        try {
          bar_aggregator->on_bar(bar);
          processed_bars++;
        } catch (...) {
          skipped_bars++;
        }
      } else {
        throw;  // Re-throw unexpected errors
      }
    }
  }

  std::cout << "Processed " << processed_bars << " bars, skipped "
            << skipped_bars << " due to gaps" << std::endl;

  // Verify we produced aggregated bars
  EXPECT_GT(aggregated_bar_count, 0) << "No aggregated bars were produced";

  // Verify indicators eventually became ready
  EXPECT_TRUE(indicators_ready) << "Indicators never became ready";

  // Verify we got a reasonable number of bars before indicators were ready
  // MACD with 26-period slow EMA should need ~26-35 bars to be ready
  EXPECT_LE(ready_after_bars, 40) << "Indicators took too long to become ready";

  // Verify we have a steady stream of indicator values after ready
  const int expected_snapshots = aggregated_bar_count - ready_after_bars + 1;
  EXPECT_EQ(snapshot_history.size(), expected_snapshots)
      << "Missing indicator snapshots after ready";

  // Verify each snapshot contains all expected indicators
  for (const auto& snapshot : snapshot_history) {
    EXPECT_EQ(snapshot.size(), 3) << "Snapshot missing indicators";
    EXPECT_TRUE(snapshot.contains("EMA")) << "Missing EMA in snapshot";
    EXPECT_TRUE(snapshot.contains("ATR")) << "Missing ATR in snapshot";
    EXPECT_TRUE(snapshot.contains("MACD")) << "Missing MACD in snapshot";

    // Verify EMA snapshot structure
    const auto& ema_values = snapshot.at("EMA");
    EXPECT_TRUE(ema_values.contains("ema"))
        << "Missing 'ema' value in EMA snapshot";
    EXPECT_GT(ema_values.at("ema"), 0.0) << "Invalid EMA value";

    // Verify ATR snapshot structure
    const auto& atr_values = snapshot.at("ATR");
    EXPECT_TRUE(atr_values.contains("atr"))
        << "Missing 'atr' value in ATR snapshot";
    EXPECT_GT(atr_values.at("atr"), 0.0) << "Invalid ATR value";

    // Verify MACD snapshot structure
    const auto& macd_values = snapshot.at("MACD");
    EXPECT_TRUE(macd_values.contains("macd"))
        << "Missing 'macd' value in MACD snapshot";
    EXPECT_TRUE(macd_values.contains("signal"))
        << "Missing 'signal' value in MACD snapshot";
    EXPECT_TRUE(macd_values.contains("histogram"))
        << "Missing 'histogram' value in MACD snapshot";
  }

  // Verify indicator values are changing (not stuck)
  if (snapshot_history.size() >= 2) {
    const auto& first_snapshot = snapshot_history.front();
    const auto& last_snapshot = snapshot_history.back();

    // At least one indicator should have different values
    bool values_changed = false;
    values_changed |= (first_snapshot.at("EMA").at("ema") !=
                       last_snapshot.at("EMA").at("ema"));
    values_changed |= (first_snapshot.at("ATR").at("atr") !=
                       last_snapshot.at("ATR").at("atr"));
    values_changed |= (first_snapshot.at("MACD").at("macd") !=
                       last_snapshot.at("MACD").at("macd"));

    EXPECT_TRUE(values_changed) << "Indicator values did not change over time";
  }

  // Log summary for visibility
  std::cout << "Integration test summary:" << std::endl;
  std::cout << "  Input bars processed: " << input_bars.size() << std::endl;
  std::cout << "  Aggregated bars produced: " << aggregated_bar_count
            << std::endl;
  std::cout << "  Indicators ready after: " << ready_after_bars
            << " aggregated bars" << std::endl;
  std::cout << "  Total indicator snapshots: " << snapshot_history.size()
            << std::endl;

  if (!snapshot_history.empty()) {
    const auto& final_snapshot = snapshot_history.back();
    std::cout << "  Final indicator values:" << std::endl;
    std::cout << "    EMA(20): " << final_snapshot.at("EMA").at("ema")
              << std::endl;
    std::cout << "    ATR(14): " << final_snapshot.at("ATR").at("atr")
              << std::endl;
    std::cout << "    MACD: " << final_snapshot.at("MACD").at("macd")
              << std::endl;
    std::cout << "    Signal: " << final_snapshot.at("MACD").at("signal")
              << std::endl;
    std::cout << "    Histogram: " << final_snapshot.at("MACD").at("histogram")
              << std::endl;
  }
}
