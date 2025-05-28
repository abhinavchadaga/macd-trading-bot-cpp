#include "BarAggregator.hpp"
#include <algorithm>

BarAggregator::BarAggregator (int aggregation_minutes)
    : _aggregation_minutes{ aggregation_minutes }
{
}

void
BarAggregator::on_bar (const Bar &input_bar)
{
  if (should_start_new_aggregation (input_bar.time ()))
    {
      if (_current_aggregated_bar.has_value ())
        {
          emit_aggregated_bar ();
        }

      _current_boundary = get_aggregation_boundary (input_bar.time ());
      _current_aggregated_bar
          = Bar{ input_bar.symbol (), input_bar.open (),  input_bar.high (),
                 input_bar.low (),    input_bar.close (), input_bar.volume (),
                 _current_boundary };
    }
  else if (_current_aggregated_bar.has_value ())
    {
      auto &current = _current_aggregated_bar.value ();
      _current_aggregated_bar
          = Bar{ current.symbol (),
                 current.open (),
                 std::max (current.high (), input_bar.high ()),
                 std::min (current.low (), input_bar.low ()),
                 input_bar.close (),
                 current.volume () + input_bar.volume (),
                 _current_boundary };
    }
}

boost::signals2::connection
BarAggregator::connect_aggregated_bar_handler (
    const aggregated_bar_signal_t::slot_type &handler)
{
  return _aggregated_bar_signal.connect (handler);
}

void
BarAggregator::emit_aggregated_bar ()
{
  if (_current_aggregated_bar.has_value ())
    {
      _aggregated_bar_signal (_current_aggregated_bar.value ());
    }
}

bool
BarAggregator::should_start_new_aggregation (
    const Bar::timestamp_t &timestamp) const
{
  if (!_current_aggregated_bar.has_value ())
    {
      return true;
    }

  return get_aggregation_boundary (timestamp) != _current_boundary;
}

Bar::timestamp_t
BarAggregator::get_aggregation_boundary (
    const Bar::timestamp_t &timestamp) const
{
  auto time_point
      = std::chrono::time_point_cast<std::chrono::minutes> (timestamp);
  auto minutes_since_epoch = time_point.time_since_epoch ().count ();

  auto aligned_minutes
      = (minutes_since_epoch / _aggregation_minutes) * _aggregation_minutes;

  return std::chrono::sys_time<std::chrono::nanoseconds>{ std::chrono::minutes{
      aligned_minutes } };
}
