#include "Bar.hpp"
#include "indicators/ohlcv/ATR.hpp"
#include "indicators/ohlcv/EMA.hpp"
#include "indicators/ohlcv/MACD.hpp"

#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <ta-lib/ta_libc.h>
#include <vector>

class IndicatorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        const TA_RetCode ret_code = TA_Initialize();
        ASSERT_EQ(ret_code, TA_SUCCESS) << "Failed to initialize TA-Lib";
    }

    void TearDown() override { TA_Shutdown(); }

    static std::vector<OHLCV> createTestData()
    {
        // open, high, low, close
        const std::vector<std::array<double, 4>> test_data{
            {100.0, 102.0, 99.0, 101.0},  {101.0, 104.0, 100.0, 103.0}, {103.0, 105.0, 102.0, 104.0},
            {104.0, 106.0, 103.0, 105.0}, {105.0, 107.0, 104.0, 106.0}, {106.0, 108.0, 105.0, 107.0},
            {107.0, 109.0, 106.0, 108.0}, {108.0, 110.0, 107.0, 109.0}, {109.0, 111.0, 108.0, 110.0},
            {110.0, 112.0, 109.0, 111.0}, {111.0, 113.0, 110.0, 112.0}, {112.0, 114.0, 111.0, 113.0},
            {113.0, 115.0, 112.0, 114.0}, {114.0, 116.0, 113.0, 115.0}, {115.0, 117.0, 114.0, 116.0},
            {116.0, 118.0, 115.0, 117.0}, {117.0, 119.0, 116.0, 118.0}, {118.0, 120.0, 117.0, 119.0},
            {119.0, 121.0, 118.0, 120.0}, {120.0, 122.0, 119.0, 121.0}};

        std::vector<OHLCV> ohlcv_data{};
        ohlcv_data.reserve(test_data.size());
        for (const auto& data : test_data)
        {
            ohlcv_data.emplace_back(data[0], data[1], data[2], data[3], 1000);
        }

        return ohlcv_data;
    }

    static void logComparison(
        const std::string& indicator,
        const int          period,
        const double       my_result,
        const double       talib_result,
        const double       threshold)
    {
        const double abs_diff{std::abs(my_result - talib_result)};

        std::cout << indicator << "-" << period << ": My=" << my_result << ", TA-Lib=" << talib_result
                  << ", Abs Diff=" << abs_diff << ", Threshold=" << threshold << std::endl;
    }
};

TEST_F(IndicatorTest, EMA_CompareWithTALib)
{
    constexpr std::size_t period{10};
    constexpr double      threshold{0.001};

    const auto          ohlcv_data{createTestData()};
    EMA                 my_ema{period};
    std::vector<double> close_prices{};

    for (std::size_t i = 0; i < ohlcv_data.size(); ++i)
    {
        close_prices.push_back(ohlcv_data[i].close);
        my_ema.write(ohlcv_data[i]);

        std::size_t current_period{i + 1};
        if (current_period >= period)
        {
            ASSERT_EQ(my_ema.is_ready(), true) << "EMA should be ready when current_period >= " << period
                                               << ", but is_ready() returned false at period " << current_period;
        }
        else
        {
            ASSERT_EQ(my_ema.is_ready(), false) << "EMA should not be ready with less than " << period
                                                << " bars, but is_ready() returned true at period " << current_period;
        }
    }

    std::vector<double> talib_ema(close_prices.size());
    int                 out_begin{}, out_nb_element{};

    const TA_RetCode ret_code{TA_EMA(
        0,
        static_cast<int>(close_prices.size()) - 1,
        close_prices.data(),
        period,
        &out_begin,
        &out_nb_element,
        talib_ema.data())};

    ASSERT_EQ(ret_code, TA_SUCCESS) << "TA-Lib EMA calculation failed";
    ASSERT_TRUE(my_ema.is_ready()) << "My EMA should be ready";

    const double my_result{my_ema.read()["ema"]};
    const double talib_value{talib_ema[out_nb_element - 1]};

    logComparison("EMA", period, my_result, talib_value, threshold);

    const double abs_diff{std::abs(my_result - talib_value)};
    EXPECT_LT(abs_diff, threshold) << "EMA absolute difference exceeds " << threshold;
}

TEST_F(IndicatorTest, ATR_CompareWithTALib)
{
    constexpr std::size_t period{14};
    constexpr double      threshold{0.001};

    const auto ohlcv_data{createTestData()};
    ATR        my_atr{period};

    std::vector<double> high_prices, low_prices, close_prices;
    for (std::size_t i = 0; i < ohlcv_data.size(); ++i)
    {
        high_prices.push_back(ohlcv_data[i].high);
        low_prices.push_back(ohlcv_data[i].low);
        close_prices.push_back(ohlcv_data[i].close);
        my_atr.write(ohlcv_data[i]);

        std::size_t current_period{i};
        if (current_period >= period)
        {
            ASSERT_EQ(my_atr.is_ready(), true) << "ATR should be ready when current_period >= " << period
                                               << ", but is_ready() returned false at period " << current_period;
        }
        else
        {
            ASSERT_EQ(my_atr.is_ready(), false) << "ATR should not be ready with less than " << period
                                                << " bars, but is_ready() returned true at period " << current_period;
        }
    }

    // Calculate TA-Lib ATR
    std::vector<double> talib_atr(close_prices.size());
    int                 out_begin, out_nb_element;

    const TA_RetCode ret_code{TA_ATR(
        0,
        static_cast<int>(close_prices.size() - 1),
        high_prices.data(),
        low_prices.data(),
        close_prices.data(),
        period,
        &out_begin,
        &out_nb_element,
        talib_atr.data())};

    ASSERT_EQ(ret_code, TA_SUCCESS) << "TA-Lib ATR calculation failed";
    ASSERT_TRUE(my_atr.is_ready()) << "My ATR should be ready";

    const double my_value{my_atr.read()["atr"]};
    const double talib_value{talib_atr[out_nb_element - 1]};

    logComparison("ATR", period, my_value, talib_value, threshold);

    const double abs_diff{std::abs(my_value - talib_value)};
    EXPECT_LT(abs_diff, threshold) << "ATR absolute difference exceeds " << threshold;
}

TEST_F(IndicatorTest, MACD_CompareWithTALib)
{
    constexpr std::size_t fast_period{3};
    constexpr std::size_t slow_period{5};
    constexpr std::size_t signal_period{2};
    constexpr double      threshold{0.01};

    const auto ohlcv_data{createTestData()};
    MACD       my_macd{fast_period, slow_period, signal_period};

    std::vector<double> close_prices{};
    for (std::size_t i = 0; i < ohlcv_data.size(); ++i)
    {
        close_prices.push_back(ohlcv_data[i].close);
        my_macd.write(ohlcv_data[i]);
        std::size_t period{i + 1};
        if (period >= slow_period + signal_period)
        {
            ASSERT_EQ(my_macd.is_ready(), true) << "MACD should be ready when period >= " << slow_period + signal_period
                                                << ", but is_ready() returned false at period " << period;
        }
        else
        {
            ASSERT_EQ(my_macd.is_ready(), false)
                << "MACD should not be ready with less than " << slow_period + signal_period
                << " bars, but is_ready() returned true at period " << period;
        }
    }

    std::vector<double> talib_macd(close_prices.size());
    std::vector<double> talib_signal(close_prices.size());
    std::vector<double> talib_histogram(close_prices.size());
    int                 out_begin, out_nb_element;

    TA_RetCode ret_code = TA_MACD(
        0,
        static_cast<int>(close_prices.size() - 1),
        close_prices.data(),
        fast_period,
        slow_period,
        signal_period,
        &out_begin,
        &out_nb_element,
        talib_macd.data(),
        talib_signal.data(),
        talib_histogram.data());

    ASSERT_EQ(ret_code, TA_SUCCESS) << "TA-Lib MACD calculation failed";
    ASSERT_TRUE(my_macd.is_ready()) << "My MACD should be ready";

    auto   my_snapshot        = my_macd.read();
    double my_macd_value      = my_snapshot["macd"];
    double my_signal_value    = my_snapshot["signal"];
    double my_histogram_value = my_snapshot["histogram"];

    double talib_macd_value      = talib_macd[out_nb_element - 1];
    double talib_signal_value    = talib_signal[out_nb_element - 1];
    double talib_histogram_value = talib_histogram[out_nb_element - 1];

    logComparison("MACD", fast_period, my_macd_value, talib_macd_value, threshold);
    logComparison("MACD-Signal", signal_period, my_signal_value, talib_signal_value, threshold);
    logComparison("MACD-Histogram", 0, my_histogram_value, talib_histogram_value, threshold);

    double macd_abs_diff{std::abs(my_macd_value - talib_macd_value)};
    double signal_abs_diff{std::abs(my_signal_value - talib_signal_value)};
    double histogram_abs_diff{std::abs(my_histogram_value - talib_histogram_value)};

    EXPECT_LT(macd_abs_diff, threshold) << "MACD absolute difference exceeds " << threshold;
    EXPECT_LT(signal_abs_diff, threshold) << "MACD Signal absolute difference exceeds " << threshold;
    EXPECT_LT(histogram_abs_diff, threshold) << "MACD Histogram absolute difference exceeds " << threshold;
}
