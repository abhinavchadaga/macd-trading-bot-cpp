#pragma once

#include "Indicator.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>

class EMA final : Indicator
{
public:
  explicit EMA (std::size_t window);

  bool ready () override;

  void push (const Bar &) override;

  std::unordered_map<std::string, double> snapshot () override;

private:
  double _value{};
  std::size_t _window{};
  std::size_t _n_periods{};
};
