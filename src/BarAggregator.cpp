#include "BarAggregator.hpp"
#include <algorithm>
#include <stdexcept>

BarAggregator::BarAggregator (int aggregation_minutes)
    : _aggregation_minutes{ aggregation_minutes }
{
}

void
BarAggregator::on_bar (const Bar &input_bar)
{
  if (_expected_next_timestamp.has_value ())
    {
      if (input_bar.time () != _expected_next_timestamp.value ())
        {
          throw std::runtime_error (
              "Bar timestamp does not match expected sequence");
        }
    }

  if (_bars_in_current_window == 0)
    {
      _current_aggregated_bar
          = Bar{ input_bar.symbol (), input_bar.open (),  input_bar.high (),
                 input_bar.low (),    input_bar.close (), input_bar.volume (),
                 input_bar.time () };
      _bars_in_current_window = 1;
    }
  else
    {
      const auto &current = _current_aggregated_bar.value ();
      _current_aggregated_bar
          = Bar{ current.symbol (),
                 current.open (),
                 std::max (current.high (), input_bar.high ()),
                 std::min (current.low (), input_bar.low ()),
                 input_bar.close (),
                 current.volume () + input_bar.volume (),
                 current.time () };
      _bars_in_current_window++;
    }

  _expected_next_timestamp = input_bar.time () + std::chrono::minutes (1);

  if (_bars_in_current_window == _aggregation_minutes)
    {
      emit_aggregated_bar ();
      _bars_in_current_window = 0;
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
