#pragma once

#include "Bar.hpp"

#include <boost/signals2.hpp>

template <std::size_t Count, ChronoDuration TimeUnit>
  requires(Count > 0)
class BarAggregator
{
  using AggregatedBar = Bar<Count, TimeUnit>;
  using aggregated_bar_signal_t
    = boost::signals2::signal<void(const AggregatedBar &)>;

public:

  explicit BarAggregator();

  void on_bar(const Bar1min &input_bar);

  boost::signals2::connection connect_aggregated_bar_handler(
    const aggregated_bar_signal_t::slot_type &handler);

private:

  void emit_aggregated_bar();

  std::optional<AggregatedBar> _current_aggregated_bar {};
  int                          _bars_in_current_window {};
  std::optional<Bar1min>       _last_input_bar {};

  aggregated_bar_signal_t _aggregated_bar_signal;
};

template <std::size_t Count, ChronoDuration TimeUnit>
  requires(Count > 0)
inline BarAggregator<Count, TimeUnit>::BarAggregator()
  : _current_aggregated_bar {}
  , _bars_in_current_window {}
  , _last_input_bar {}
  , _aggregated_bar_signal {}
{
}

template <std::size_t Count, ChronoDuration TimeUnit>
  requires(Count > 0)
inline void
BarAggregator<Count, TimeUnit>::on_bar(const Bar1min &input_bar)
{
  constexpr auto target_duration = AggregatedBar::duration();
  constexpr auto input_duration  = Bar1min::duration();
  static_assert(
    target_duration.count() % input_duration.count() == 0,
    "target_duration must be evenly divisible by "
    "input_duration");
  constexpr int bars_needed = target_duration / input_duration;

  if (_last_input_bar.has_value())
    {
      if (!isConsecutive(_last_input_bar.value(), input_bar))
        {
          throw std::runtime_error(
            "Bar timestamp does not match expected sequence");
        }
    }

  if (_bars_in_current_window == 0)
    {
      auto aggregated_timestamp
        = std::chrono::time_point_cast<TimeUnit>(input_bar.timestamp());
      _current_aggregated_bar
        = AggregatedBar { input_bar.symbol(),  input_bar.open(),
                          input_bar.high(),    input_bar.low(),
                          input_bar.close(),   input_bar.volume(),
                          aggregated_timestamp };
      _bars_in_current_window = 1;
    }
  else
    {
      const auto &current = _current_aggregated_bar.value();
      _current_aggregated_bar
        = AggregatedBar { current.symbol(),
                          current.open(),
                          std::max(current.high(), input_bar.high()),
                          std::min(current.low(), input_bar.low()),
                          input_bar.close(),
                          current.volume() + input_bar.volume(),
                          current.timestamp() };
      _bars_in_current_window++;
    }

  _last_input_bar = input_bar;

  if (_bars_in_current_window == bars_needed)
    {
      emit_aggregated_bar();
      _bars_in_current_window = 0;
    }
}

template <std::size_t Count, ChronoDuration TimeUnit>
  requires(Count > 0)
inline boost::signals2::connection
BarAggregator<Count, TimeUnit>::connect_aggregated_bar_handler(
  const aggregated_bar_signal_t::slot_type &handler)
{
  return _aggregated_bar_signal.connect(handler);
}

template <std::size_t Count, ChronoDuration TimeUnit>
  requires(Count > 0)
inline void
BarAggregator<Count, TimeUnit>::emit_aggregated_bar()
{
  if (_current_aggregated_bar.has_value())
    {
      _aggregated_bar_signal(_current_aggregated_bar.value());
    }
}
