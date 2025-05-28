#pragma once

#include <chrono>
#include <string>

class bar
{
public:
  using timestamp_t = std::chrono::sys_time<std::chrono::nanoseconds>;

  bar (std::string symbol, const double open, const double high,
       const double low, const double close, const uint64_t volume,
       const timestamp_t ts)
      : _symbol{ std::move (symbol) }, _open{ open }, _high{ high },
        _low{ low }, _close{ close }, _volume{ volume }, _timestamp{ ts }
  {
  }

  [[nodiscard]] std::string
  symbol () const
  {
    return _symbol;
  }

  [[nodiscard]] double
  open () const
  {
    return _open;
  }

  [[nodiscard]] double
  high () const
  {
    return _high;
  }

  [[nodiscard]] double
  low () const
  {
    return _low;
  }

  [[nodiscard]] double
  close () const
  {
    return _close;
  }

  [[nodiscard]] uint64_t
  volume () const
  {
    return _volume;
  }

  [[nodiscard]] timestamp_t
  time () const
  {
    return _timestamp;
  }

private:
  std::string _symbol{};
  double _open{};
  double _high{};
  double _low{};
  double _close{};
  uint64_t _volume{};
  timestamp_t _timestamp{};
};
