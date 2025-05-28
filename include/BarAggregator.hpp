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

  [[nodiscard]] bool
  should_start_new_aggregation (const Bar::timestamp_t &timestamp) const;

  [[nodiscard]] Bar::timestamp_t
  get_aggregation_boundary (const Bar::timestamp_t &timestamp) const;

  int _aggregation_minutes;
  std::optional<Bar> _current_aggregated_bar{};
  Bar::timestamp_t _current_boundary{};

  aggregated_bar_signal_t _aggregated_bar_signal;
};
