#pragma once

#include <cstddef>

class ScalarEMA
{
public:
  explicit ScalarEMA (std::size_t window);

  void push (double value);

  double value () const;

  bool ready () const;

private:
  double _value{};
  std::size_t _window;
  std::size_t _n_periods{};
};
