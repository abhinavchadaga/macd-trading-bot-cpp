#include "Bar.hpp"

#include <chrono>
#include <gtest/gtest.h>

#define START_TIME 1000

using namespace std::chrono;

class BarTest : public ::testing::Test
{
protected:

  void
  SetUp() override
  {
    base_time = sys_time<minutes> { minutes { START_TIME } };
  }

  sys_time<minutes> base_time;
};

TEST_F(BarTest, ConsecutiveSameDurationBars)
{
  bar_1min bar1 { "AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time };
  bar_1min bar2 {
    "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, base_time + minutes { 1 }
  };

  EXPECT_TRUE(is_consecutive(bar1, bar2));
  EXPECT_TRUE(is_consecutive(bar2, bar1));
}

TEST_F(BarTest, ConsecutiveDifferentDurationBars)
{
  bar_1min bar_1min { "AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time };
  bar_5min bar_5min {
    "AAPL", 103.0, 108.0, 102.0, 106.0, 5000, base_time + minutes { 1 }
  };

  EXPECT_TRUE(is_consecutive(bar_1min, bar_5min));
  EXPECT_TRUE(is_consecutive(bar_5min, bar_1min));
}

TEST_F(BarTest, NonConsecutiveBarsWithGap)
{
  bar_1min bar1 { "AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time };
  bar_1min bar2 {
    "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, base_time + minutes { 3 }
  };

  EXPECT_FALSE(is_consecutive(bar1, bar2));
  EXPECT_FALSE(is_consecutive(bar2, bar1));
}

TEST_F(BarTest, OverlappingBars)
{
  bar_5min bar1 { "AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time };
  bar_1min bar2 {
    "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, base_time + minutes { 2 }
  };

  EXPECT_FALSE(is_consecutive(bar1, bar2));
  EXPECT_FALSE(is_consecutive(bar2, bar1));
}

TEST_F(BarTest, SameTimestampBars)
{
  bar_1min bar1 { "AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time };
  bar_1min bar2 { "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, base_time };

  EXPECT_FALSE(is_consecutive(bar1, bar2));
  EXPECT_FALSE(is_consecutive(bar2, bar1));
}

TEST_F(BarTest, DifferentSymbols)
{
  bar_1min bar1 { "AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time };
  bar_1min bar2 {
    "MSFT", 103.0, 106.0, 102.0, 104.0, 1200, base_time + minutes { 1 }
  };

  EXPECT_FALSE(is_consecutive(bar1, bar2));
  EXPECT_FALSE(is_consecutive(bar2, bar1));
}

TEST_F(BarTest, CrossDurationConsecutive)
{
  bar_1min bar_1min { "AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time };
  Bar<2, minutes> bar_2min {
    "AAPL", 103.0, 108.0, 102.0, 106.0, 2500, base_time + minutes { 1 }
  };

  EXPECT_TRUE(is_consecutive(bar_1min, bar_2min));
  EXPECT_TRUE(is_consecutive(bar_2min, bar_1min));
}

TEST_F(BarTest, HourlyAndMinuteConsecutive)
{
  bar_5min bar_5min { "AAPL",
                    100.0,
                    105.0,
                    99.0,
                    103.0,
                    5000,
                    sys_time<minutes> { minutes { 955 } } };
  bar_1h   bar_1h {
    "AAPL", 103.0, 108.0, 102.0, 106.0, 60000, sys_time<hours> { hours { 16 } }
  };

  EXPECT_TRUE(is_consecutive(bar_5min, bar_1h));
  EXPECT_TRUE(is_consecutive(bar_1h, bar_5min));
}

TEST_F(BarTest, LargeDifferentDurationNonConsecutive)
{
  bar_1min bar_1min { "AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time };
  bar_1h   bar_1h { "AAPL",
                103.0,
                108.0,
                102.0,
                106.0,
                60000,
                time_point_cast<hours>(base_time + minutes { 30 }) };

  EXPECT_FALSE(is_consecutive(bar_1min, bar_1h));
  EXPECT_FALSE(is_consecutive(bar_1h, bar_1min));
}

TEST_F(BarTest, EdgeCaseVerySmallGap)
{
  auto precise_time = sys_time<nanoseconds> { nanoseconds { 1000000000000 } };
  Bar<1, nanoseconds> bar1 { "AAPL", 100.0, 105.0,       99.0,
                             103.0,  1000,  precise_time };
  Bar<1, nanoseconds> bar2 {
    "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, precise_time + nanoseconds { 2 }
  };

  EXPECT_FALSE(is_consecutive(bar1, bar2));
}

TEST_F(BarTest, EdgeCasePreciseConsecutive)
{
  auto precise_time = sys_time<nanoseconds> { nanoseconds { 1000000000000 } };
  Bar<1, nanoseconds> bar1 { "AAPL", 100.0, 105.0,       99.0,
                             103.0,  1000,  precise_time };
  Bar<1, nanoseconds> bar2 {
    "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, precise_time + nanoseconds { 1 }
  };

  EXPECT_TRUE(is_consecutive(bar1, bar2));
  EXPECT_TRUE(is_consecutive(bar2, bar1));
}
