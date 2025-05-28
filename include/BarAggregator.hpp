#pragma once

#include "types.hpp"
#include <boost/signals2.hpp>
#include <optional>

class BarAggregator
{
public:
  using aggregated_bar_signal_t = boost::signals2::signal<void (const Bar &)>;

  explicit BarAggregator (int aggregation_minutes = 5);

  void on_bar (const Bar &input_bar);

  boost::signals2::connection connect_aggregated_bar_handler (
      const aggregated_bar_signal_t::slot_type &handler);

private:
  void emit_aggregated_bar ();

  int _aggregation_minutes;
  std::optional<Bar> _current_aggregated_bar{};
  int _bars_in_current_window{};
  std::optional<Bar::timestamp_t> _expected_next_timestamp{};

  aggregated_bar_signal_t _aggregated_bar_signal;
};
