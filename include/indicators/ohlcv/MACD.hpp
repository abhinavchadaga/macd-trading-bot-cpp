#pragma once

#include "EMA.hpp"
#include "IndicatorConfig.hpp"
#include "OHLCVIndicator.hpp"

#include <string_view>

class MACD final : public OHLCVIndicator
{
public:
    static constexpr std::string_view name{"MACD"};

    explicit MACD(std::size_t fast_period = 12, std::size_t slow_period = 26, std::size_t signal_period = 9);

    explicit MACD(const IndicatorConfig& config);

    //
    // Indicator methods

    [[nodiscard]]
    bool is_ready() const override;

    [[nodiscard]]
    Snapshot read() const override;

    void write(const OHLCV& ohlcv) override;

private:
    EMA _fast_ema;
    EMA _slow_ema;
    EMA _signal_ema;
};
