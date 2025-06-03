#pragma once

#include <chrono>
#include <concepts>
#include <cstddef>
#include <string>

struct ohlcv
{
  double   open {};
  double   high {};
  double   low {};
  double   close {};
  uint64_t volume {};
};

template <typename T>
concept chrono_duration
  = std::same_as<T, std::chrono::nanoseconds>
 || std::same_as<T, std::chrono::microseconds>
 || std::same_as<T, std::chrono::milliseconds>
 || std::same_as<T, std::chrono::seconds>
 || std::same_as<T, std::chrono::minutes>
 || std::same_as<T, std::chrono::hours> || std::same_as<T, std::chrono::days>
 || std::same_as<T, std::chrono::weeks> || std::same_as<T, std::chrono::months>
 || std::same_as<T, std::chrono::years>;

template <std::size_t Count, chrono_duration TimeUnit>
  requires(Count > 0)
class bar
{
public:

  using timestamp = std::chrono::sys_time<TimeUnit>;

  static constexpr int
  count()
  {
    return Count;
  }

  static constexpr TimeUnit
  duration()
  {
    return TimeUnit { Count };
  }

  bar(std::string symbol, const ohlcv &ohlcv, const timestamp ts)
    : _symbol { std::move(symbol) }
    , _ohlcv { ohlcv }
    , _timestamp { ts }
  {
  }

  bar(
    std::string     symbol,
    const double    open,
    const double    high,
    const double    low,
    const double    close,
    const uint64_t  volume,
    const timestamp ts)
    : _symbol { std::move(symbol) }
    , _ohlcv { open, high, low, close, volume }
    , _timestamp { ts }
  {
  }

  [[nodiscard]]
  const std::string &
  symbol() const
  {
    return _symbol;
  }

  [[nodiscard]]
  const ohlcv &
  ohlcv() const
  {
    return _ohlcv;
  }

  [[nodiscard]]
  double
  open() const
  {
    return _ohlcv.open;
  }

  [[nodiscard]]
  double
  high() const
  {
    return _ohlcv.high;
  }

  [[nodiscard]]
  double
  low() const
  {
    return _ohlcv.low;
  }

  [[nodiscard]]
  double
  close() const
  {
    return _ohlcv.close;
  }

  [[nodiscard]]
  uint64_t
  volume() const
  {
    return _ohlcv.volume;
  }

  [[nodiscard]]
  timestamp
  timestamp() const
  {
    return _timestamp;
  }

private:

  std::string _symbol {};
  ohlcv       _ohlcv {};
  timestamp   _timestamp {};
};

template <
  std::size_t       Count1,
  chrono_duration   TimeUnit1,
  std::size_t       Count2,
  chrono_duration   TimeUnit2>
bool
is_consecutive(
  const bar<Count1, TimeUnit1> &left,
  const bar<Count2, TimeUnit2> &right)
{
  if (left.symbol() != right.symbol())
    {
      return false;
    }

  auto left_start
    = std::chrono::time_point_cast<std::chrono::nanoseconds>(left.timestamp());
  auto left_end
    = left_start
    + std::chrono::duration_cast<std::chrono::nanoseconds>(left.duration());

  auto right_start = std::chrono::time_point_cast<std::chrono::nanoseconds>(
    right.timestamp());
  auto right_end
    = right_start
    + std::chrono::duration_cast<std::chrono::nanoseconds>(right.duration());

  return (left_end == right_start) || (right_end == left_start);
}

using bar_1min = bar<1, std::chrono::minutes>;
using bar_5min = bar<5, std::chrono::minutes>;
using bar_1h   = bar<1, std::chrono::hours>;
