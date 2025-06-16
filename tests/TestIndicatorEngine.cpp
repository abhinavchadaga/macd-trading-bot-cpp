#include "Bar.hpp"
#include "BarAggregator.hpp"
#include "IndicatorConfig.hpp"
#include "IndicatorEngine.hpp"
#include "Utils.hpp"

#include <cstdlib>
#include <filesystem>
#include <gtest/gtest.h>
#include <vector>

class IndicatorEngineIntegrationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Set up indicator configurations
        IndicatorConfig ema_config{.name = "EMA", .params = {{"period", 20}}};

        IndicatorConfig atr_config{.name = "ATR", .params = {{"period", 14}}};

        IndicatorConfig macd_config{
            .name = "MACD", .params = {{"fast_period", 12}, {"slow_period", 26}, {"signal_period", 9}}};

        indicator_configs = {ema_config, atr_config, macd_config};

        // Initialize components
        bar_aggregator   = std::make_unique<BarAggregator<5, std::chrono::minutes>>();
        indicator_engine = std::make_unique<DefaultIndicatorEngine>(indicator_configs);

        // Connect aggregator to indicator engine
        aggregator_connection = bar_aggregator->subscribe([this](const Bar5min& aggregated_bar)
                                                          { this->on_aggregated_bar(aggregated_bar); });

        // Connect to indicator updates
        indicator_connection = indicator_engine->subscribe([this](const DefaultIndicatorEngine::Snapshots& snapshots)
                                                           { this->on_indicator_update(snapshots); });
    }

    void TearDown() override
    {
        aggregator_connection.disconnect();
        indicator_connection.disconnect();
    }

    void on_indicator_update(const DefaultIndicatorEngine::Snapshots& snapshots)
    {
        if (!indicators_ready)
        {
            indicators_ready       = true;
            first_indicator_update = aggregated_bar_count;
        }
        snapshot_history.push_back(snapshots);
    }

    void on_aggregated_bar(const Bar5min& bar)
    {
        ++aggregated_bar_count;
        indicator_engine->on_bar(bar);
    }

    std::vector<IndicatorConfig>                            indicator_configs;
    std::unique_ptr<BarAggregator<5, std::chrono::minutes>> bar_aggregator;
    std::unique_ptr<DefaultIndicatorEngine>                 indicator_engine;
    boost::signals2::connection                             aggregator_connection;
    boost::signals2::connection                             indicator_connection;

    // Test tracking variables
    int                                            aggregated_bar_count{0};
    bool                                           indicators_ready{false};
    int                                            first_indicator_update{0};
    std::vector<DefaultIndicatorEngine::Snapshots> snapshot_history;
};

TEST_F(IndicatorEngineIntegrationTest, ProcessesPLTRDataAndProducesSteadyIndicatorStream)
{
    std::string csv_path = "/tmp/pltr_one_day_data.csv";
    std::string cmd{
        "historical_bars_to_csv PLTR 2025-05-30 "
        "2025-05-31 --output " +
        csv_path};
    std::system(cmd.c_str());
    ASSERT_TRUE(std::filesystem::exists(csv_path)) << "PLTR CSV file not found at: " << csv_path;

    auto input_bars = createBarsFromCSV(csv_path);
    ASSERT_GT(input_bars.size(), 0) << "No bars loaded from CSV";

    int processed_bars = 0;
    for (const auto& bar : input_bars)
    {
        bar_aggregator->on_bar(bar);
        processed_bars++;
    }

    std::cout << "Processed " << processed_bars << " bars" << std::endl;

    EXPECT_GT(aggregated_bar_count, 0) << "No aggregated bars were produced";
    EXPECT_TRUE(indicators_ready) << "Indicators never became ready";

    // Indicator Engine should be ready after receiving the 35th aggregated bar
    EXPECT_EQ(first_indicator_update, 35) << "Indicators did not become ready after expected bars";

    const int expected_snapshots = aggregated_bar_count - first_indicator_update + 1;
    EXPECT_EQ(snapshot_history.size(), expected_snapshots) << "Missing indicator snapshots after ready";

    // Verify each snapshot contains all expected indicators
    for (const auto& snapshot : snapshot_history)
    {
        EXPECT_EQ(snapshot.size(), 3) << "Snapshot missing indicators";
        EXPECT_TRUE(snapshot.contains("EMA")) << "Missing EMA in snapshot";
        EXPECT_TRUE(snapshot.contains("ATR")) << "Missing ATR in snapshot";
        EXPECT_TRUE(snapshot.contains("MACD")) << "Missing MACD in snapshot";

        // Verify EMA snapshot structure
        const auto& ema_values = snapshot.at("EMA");
        EXPECT_TRUE(ema_values.contains("ema")) << "Missing 'ema' value in EMA snapshot";
        EXPECT_GT(ema_values.at("ema"), 0.0) << "Invalid EMA value";

        // Verify ATR snapshot structure
        const auto& atr_values = snapshot.at("ATR");
        EXPECT_TRUE(atr_values.contains("atr")) << "Missing 'atr' value in ATR snapshot";
        EXPECT_GT(atr_values.at("atr"), 0.0) << "Invalid ATR value";

        // Verify MACD snapshot structure
        const auto& macd_values = snapshot.at("MACD");
        EXPECT_TRUE(macd_values.contains("macd")) << "Missing 'macd' value in MACD snapshot";
        EXPECT_TRUE(macd_values.contains("signal")) << "Missing 'signal' value in MACD snapshot";
        EXPECT_TRUE(macd_values.contains("histogram")) << "Missing 'histogram' value in MACD snapshot";
    }

    // Verify indicator values are changing (not stuck)
    if (snapshot_history.size() >= 2)
    {
        const auto& first_snapshot = snapshot_history.front();
        const auto& last_snapshot  = snapshot_history.back();

        // At least one indicator should have different values
        bool values_changed = false;
        values_changed |= (first_snapshot.at("EMA").at("ema") != last_snapshot.at("EMA").at("ema"));
        values_changed |= (first_snapshot.at("ATR").at("atr") != last_snapshot.at("ATR").at("atr"));
        values_changed |= (first_snapshot.at("MACD").at("macd") != last_snapshot.at("MACD").at("macd"));

        EXPECT_TRUE(values_changed) << "Indicator values did not change over time";
    }

    // Log summary for visibility
    std::cout << "Integration test summary:" << std::endl;
    std::cout << "  Input bars processed: " << input_bars.size() << std::endl;
    std::cout << "  Aggregated bars produced: " << aggregated_bar_count << std::endl;
    std::cout << "  Indicators ready after: " << first_indicator_update << " aggregated bars" << std::endl;
    std::cout << "  Total indicator snapshots: " << snapshot_history.size() << std::endl;

    if (!snapshot_history.empty())
    {
        const auto& final_snapshot = snapshot_history.back();
        std::cout << "  Final indicator values:" << std::endl;
        std::cout << "    EMA(20): " << final_snapshot.at("EMA").at("ema") << std::endl;
        std::cout << "    ATR(14): " << final_snapshot.at("ATR").at("atr") << std::endl;
        std::cout << "    MACD: " << final_snapshot.at("MACD").at("macd") << std::endl;
        std::cout << "    Signal: " << final_snapshot.at("MACD").at("signal") << std::endl;
        std::cout << "    Histogram: " << final_snapshot.at("MACD").at("histogram") << std::endl;
    }

    std::filesystem::remove(csv_path);
}
