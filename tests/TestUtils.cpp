#include "Bar.hpp"
#include "Utils.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

using namespace std::chrono;

class UtilsTest : public ::testing::Test
{
protected:

  static void
  SetUpTestSuite()
  {
    test_csv_file_path = "/tmp/test_bars_utils.csv";

    // Generate test data using the Python executable
    std::string command
      = "historical_bars_to_csv PLTR 2024-01-02 "
        "2024-01-03 --limit 10 --output "
      + test_csv_file_path;
    int result = std::system(command.c_str());
    ASSERT_EQ(result, 0) << "Failed to generate test CSV data";
  }

  static void
  TearDownTestSuite()
  {
    std::remove(test_csv_file_path.c_str());
  }

  static std::string test_csv_file_path;
};

std::string UtilsTest::test_csv_file_path;

TEST_F(UtilsTest, ParseRFC3339UTCTimestamp)
{
  std::string timestamp = "2025-05-19T13:30:00Z";
  auto        result    = parseRFC3339UTCTimestamp(timestamp);
  auto        expected  = sys_time<minutes> { minutes { 1747661400 / 60 } };
  EXPECT_EQ(result, expected);
}

TEST_F(UtilsTest, CreateBarFromCSVLine)
{
  std::ifstream file { test_csv_file_path };
  std::string   header;
  std::getline(file, header); // Skip header
  std::string line;
  std::getline(file, line);
  file.close();

  auto bar = createBarFromCSVLine(line);

  auto expected_timestamp
    = parseRFC3339UTCTimestamp("2024-01-02 14:30:00+00:00");
  Bar1min expected_bar {
    "PLTR", 16.95, 16.95, 16.71, 16.7801, 2165968, expected_timestamp
  };

  EXPECT_EQ(bar, expected_bar);
}

TEST_F(UtilsTest, CreateBarsFromCSV)
{
  auto bars = createBarsFromCSV(test_csv_file_path);

  EXPECT_EQ(bars.size(), 10);

  std::vector<Bar1min> expected_bars = {
    { "PLTR",
     16.95,  16.95,
     16.71, 16.7801,
     2165968, parseRFC3339UTCTimestamp("2024-01-02 14:30:00+00:00") },
    { "PLTR",
     16.78, 16.785,
     16.67,   16.67,
     281221, parseRFC3339UTCTimestamp("2024-01-02 14:31:00+00:00") },
    { "PLTR",
     16.68,  16.73,
     16.55, 16.7201,
     804156, parseRFC3339UTCTimestamp("2024-01-02 14:32:00+00:00") },
    { "PLTR",
     16.72,  16.76,
     16.665,  16.705,
     293423, parseRFC3339UTCTimestamp("2024-01-02 14:33:00+00:00") },
    { "PLTR",
     16.72,  16.74,
     16.69, 16.7099,
     243193, parseRFC3339UTCTimestamp("2024-01-02 14:34:00+00:00") },
    { "PLTR",
     16.7012,  16.77,
     16.69,  16.755,
     276583, parseRFC3339UTCTimestamp("2024-01-02 14:35:00+00:00") },
    { "PLTR",
     16.755,  16.77,
     16.72, 16.7233,
     160855, parseRFC3339UTCTimestamp("2024-01-02 14:36:00+00:00") },
    { "PLTR",
     16.73, 16.755,
     16.715,  16.735,
     171763, parseRFC3339UTCTimestamp("2024-01-02 14:37:00+00:00") },
    { "PLTR",
     16.74,  16.75,
     16.67,  16.685,
     183376, parseRFC3339UTCTimestamp("2024-01-02 14:38:00+00:00") },
    { "PLTR",
     16.685,   16.7,
     16.66,    16.7,
     256992, parseRFC3339UTCTimestamp("2024-01-02 14:39:00+00:00") }
  };

  for (size_t i = 0; i < expected_bars.size(); ++i)
    {
      EXPECT_EQ(bars[i], expected_bars[i])
        << "Bar " << i << " does not match expected value";
    }
}
