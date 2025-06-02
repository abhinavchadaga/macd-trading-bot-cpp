#include <gtest/gtest.h>
#include <chrono>
#include "Bar.hpp"

using namespace std::chrono;

class BarTest : public ::testing::Test {
 protected:
  void SetUp() override { base_time = sys_time<minutes>{minutes{1000}}; }

  sys_time<minutes> base_time;
};

TEST_F(BarTest, ConsecutiveSameDurationBars) {
  Bar1min bar1{"AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time};
  Bar1min bar2{
      "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, base_time + minutes{1}};

  EXPECT_TRUE(isConsecutive(bar1, bar2));
  EXPECT_TRUE(isConsecutive(bar2, bar1));
}

TEST_F(BarTest, ConsecutiveDifferentDurationBars) {
  Bar1min bar1min{"AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time};
  Bar5min bar5min{
      "AAPL", 103.0, 108.0, 102.0, 106.0, 5000, base_time + minutes{1}};

  EXPECT_TRUE(isConsecutive(bar1min, bar5min));
  EXPECT_TRUE(isConsecutive(bar5min, bar1min));
}

TEST_F(BarTest, NonConsecutiveBarsWithGap) {
  Bar1min bar1{"AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time};
  Bar1min bar2{
      "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, base_time + minutes{3}};

  EXPECT_FALSE(isConsecutive(bar1, bar2));
  EXPECT_FALSE(isConsecutive(bar2, bar1));
}

TEST_F(BarTest, OverlappingBars) {
  Bar5min bar1{"AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time};
  Bar1min bar2{
      "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, base_time + minutes{2}};

  EXPECT_FALSE(isConsecutive(bar1, bar2));
  EXPECT_FALSE(isConsecutive(bar2, bar1));
}

TEST_F(BarTest, SameTimestampBars) {
  Bar1min bar1{"AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time};
  Bar1min bar2{"AAPL", 103.0, 106.0, 102.0, 104.0, 1200, base_time};

  EXPECT_FALSE(isConsecutive(bar1, bar2));
  EXPECT_FALSE(isConsecutive(bar2, bar1));
}

TEST_F(BarTest, DifferentSymbols) {
  Bar1min bar1{"AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time};
  Bar1min bar2{
      "MSFT", 103.0, 106.0, 102.0, 104.0, 1200, base_time + minutes{1}};

  EXPECT_FALSE(isConsecutive(bar1, bar2));
  EXPECT_FALSE(isConsecutive(bar2, bar1));
}

TEST_F(BarTest, CrossDurationConsecutive) {
  Bar1min bar1min{"AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time};
  Bar<2, minutes> bar2min{
      "AAPL", 103.0, 108.0, 102.0, 106.0, 2500, base_time + minutes{1}};

  EXPECT_TRUE(isConsecutive(bar1min, bar2min));
  EXPECT_TRUE(isConsecutive(bar2min, bar1min));
}

TEST_F(BarTest, HourlyAndMinuteConsecutive) {
  Bar5min bar5min{
      "AAPL", 100.0, 105.0, 99.0, 103.0, 5000, sys_time<minutes>{minutes{955}}};
  Bar1h bar1h{
      "AAPL", 103.0, 108.0, 102.0, 106.0, 60000, sys_time<hours>{hours{16}}};

  EXPECT_TRUE(isConsecutive(bar5min, bar1h));
  EXPECT_TRUE(isConsecutive(bar1h, bar5min));
}

TEST_F(BarTest, LargeDifferentDurationNonConsecutive) {
  Bar1min bar1min{"AAPL", 100.0, 105.0, 99.0, 103.0, 1000, base_time};
  Bar1h bar1h{"AAPL",
              103.0,
              108.0,
              102.0,
              106.0,
              60000,
              time_point_cast<hours>(base_time + minutes{30})};

  EXPECT_FALSE(isConsecutive(bar1min, bar1h));
  EXPECT_FALSE(isConsecutive(bar1h, bar1min));
}

TEST_F(BarTest, EdgeCaseVerySmallGap) {
  auto precise_time = sys_time<nanoseconds>{nanoseconds{1000000000000}};
  Bar<1, nanoseconds> bar1{"AAPL", 100.0, 105.0,       99.0,
                           103.0,  1000,  precise_time};
  Bar<1, nanoseconds> bar2{
      "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, precise_time + nanoseconds{2}};

  EXPECT_FALSE(isConsecutive(bar1, bar2));
}

TEST_F(BarTest, EdgeCasePreciseConsecutive) {
  auto precise_time = sys_time<nanoseconds>{nanoseconds{1000000000000}};
  Bar<1, nanoseconds> bar1{"AAPL", 100.0, 105.0,       99.0,
                           103.0,  1000,  precise_time};
  Bar<1, nanoseconds> bar2{
      "AAPL", 103.0, 106.0, 102.0, 104.0, 1200, precise_time + nanoseconds{1}};

  EXPECT_TRUE(isConsecutive(bar1, bar2));
  EXPECT_TRUE(isConsecutive(bar2, bar1));
}
