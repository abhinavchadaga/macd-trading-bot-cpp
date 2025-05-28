#pragma once

#include "types.hpp"
#include <boost/signals2.hpp>
#include <chrono>
#include <optional>
#include <string>

class BarAggregator
{
public:
  using aggregated_bar_signal_t = boost::signals2::signal<void (const bar &)>;

  explicit BarAggregator (int aggregation_minutes = 5);

  void add_bar (const bar &input_bar);

  boost::signals2::connection
  connect_aggregated_bar_handler (
      const aggregated_bar_signal_t::slot_type &handler);

private:
  void emit_aggregated_bar ();

  [[nodiscard]] bool should_start_new_aggregation (
      const bar::timestamp_t &timestamp) const;

  [[nodiscard]] bar::timestamp_t get_aggregation_boundary (
      const bar::timestamp_t &timestamp) const;

  int _aggregation_minutes;
  std::optional<bar> _current_aggregated_bar{};
  bar::timestamp_t _current_boundary{};

  aggregated_bar_signal_t _aggregated_bar_signal;
};