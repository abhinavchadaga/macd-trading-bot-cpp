#pragma once

#include "Bar.hpp"

#include <string>
#include <unordered_map>

class Indicator
{
public:
  virtual ~Indicator () = default;

  virtual bool ready () = 0;
  virtual void push (const Bar &) = 0;
  virtual std::unordered_map<std::string, double> snapshot () = 0;
};
