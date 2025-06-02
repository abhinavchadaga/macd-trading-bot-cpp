#include "Bar.hpp"
#include "Utils.hpp"

#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

using namespace std::chrono;

class UtilsTest : public ::testing::Test
{
protected:

  void
  SetUp() override
  {
    test_csv_content =
      R"(symbol,timestamp,open,high,low,close,volume,trade_count,vwap,timestamp_et
PLTR,2025-05-19T13:30:00Z,23.45,23.67,23.40,23.60,1000,10,23.55,2025-05-19T09:30:00-04:00
PLTR,2025-05-19T13:31:00Z,23.60,23.75,23.58,23.70,1200,12,23.68,2025-05-19T09:31:00-04:00)";
  }

  std::string test_csv_content;
};

TEST_F(UtilsTest, ParseRFC3339UTCTimestamp)
{
  std::string timestamp = "2025-05-19T13:30:00Z";
  auto        result    = parseRFC3339UTCTimestamp(timestamp);
  auto        expected  = sys_time<minutes> { minutes { 1747661400 / 60 } };
  EXPECT_EQ(result, expected);
}

TEST_F(UtilsTest, CreateBarFromCSVLine)
{
  std::string line
    = "PLTR,2025-05-19T13:30:00Z,23.45,23.67,23.40,23.60,1000,10,23.55,2025-"
      "05-"
      "19T09:30:00-04:00";
  auto bar = createBarFromCSVLine(line);

  EXPECT_EQ(bar.symbol(), "PLTR");
  EXPECT_DOUBLE_EQ(bar.open(), 23.45);
  EXPECT_DOUBLE_EQ(bar.high(), 23.67);
  EXPECT_DOUBLE_EQ(bar.low(), 23.40);
  EXPECT_DOUBLE_EQ(bar.close(), 23.60);
  EXPECT_EQ(bar.volume(), 1000);

  auto expected_timestamp = parseRFC3339UTCTimestamp("2025-05-19T13:30:00Z");
  EXPECT_EQ(bar.timestamp(), expected_timestamp);
}

TEST_F(UtilsTest, CreateBarsFromCSV)
{
  std::string   test_file_path = "/tmp/test_bars.csv";
  std::ofstream file { test_file_path };
  file << test_csv_content;
  file.close();

  auto bars = createBarsFromCSV(test_file_path);

  EXPECT_EQ(bars.size(), 2);

  EXPECT_EQ(bars[0].symbol(), "PLTR");
  EXPECT_DOUBLE_EQ(bars[0].open(), 23.45);
  EXPECT_DOUBLE_EQ(bars[0].close(), 23.60);
  EXPECT_EQ(bars[0].volume(), 1000);

  EXPECT_EQ(bars[1].symbol(), "PLTR");
  EXPECT_DOUBLE_EQ(bars[1].open(), 23.60);
  EXPECT_DOUBLE_EQ(bars[1].close(), 23.70);
  EXPECT_EQ(bars[1].volume(), 1200);

  std::remove(test_file_path.c_str());
}
