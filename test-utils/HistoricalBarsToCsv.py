#!/usr/bin/env python3
"""Script to fetch historical 1-minute bar data from Alpaca and save to CSV. Usage: python HistoricalBarsToCsv.py AAPL 2024-01-01 2024-01-31"""

import argparse
import os
from datetime import datetime, time
from typing import Optional

import pandas as pd
from alpaca.data import BarSet
from alpaca.data.historical import StockHistoricalDataClient
from alpaca.data.requests import StockBarsRequest
from alpaca.data.timeframe import TimeFrame, TimeFrameUnit
from cprint import cprint
from dotenv import load_dotenv

load_dotenv()


def fetch_historical_data(
    symbol: str,
    start_date: str,
    end_date: str,
    api_key: str,
    secret_key: str,
    output_file: Optional[str] = None,
    market_hours_only: bool = True,
) -> None:
    """Fetch historical 1-minute bar data and save to CSV."""

    client = StockHistoricalDataClient(api_key, secret_key)
    start_dt = datetime.strptime(start_date, "%Y-%m-%d")
    end_dt = datetime.strptime(end_date, "%Y-%m-%d")

    request_params = StockBarsRequest(
        symbol_or_symbols=[symbol],
        timeframe=TimeFrame(1, TimeFrameUnit.Minute),
        start=start_dt,
        end=end_dt,
    )

    cprint.info(f"Fetching {symbol} data from {start_date} to {end_date}...")

    bars = client.get_stock_bars(request_params)
    if not isinstance(bars, BarSet):
        raise ValueError("Unexpected return type from get_stock_bars")

    df: pd.DataFrame = bars.df

    if df.empty:
        cprint.err("No data found for the specified period.", interrupt=True)

    df = df.reset_index()

    if market_hours_only:
        cprint.info(
            "Filtering to regular market hours (9:30 AM-4 PM ET, weekdays only)…"
        )

        if "timestamp" not in df.columns:
            raise ValueError("No timestamp column found in data")

        ts_et = pd.to_datetime(df["timestamp"])
        ts_et = ts_et.dt.tz_convert("America/New_York")

        # ----- weekday filter (Mon-Fri) -----
        weekday_mask = ts_et.dt.weekday < 5

        # ----- time-of-day filter (09:30-16:00 ET) -----
        market_open = time(9, 30)
        market_close = time(16, 0)
        time_mask = (ts_et.dt.time >= market_open) & (ts_et.dt.time <= market_close)

        # apply masks
        df = df[weekday_mask & time_mask]

        df.loc[:, "timestamp"] = ts_et.loc[df.index].dt.tz_convert("UTC")

    if df.empty:
        cprint.warn("No data found after filtering.")
        return

    # Generate output filename if not provided
    if output_file is None:
        hours_suffix = "_market_hours" if market_hours_only else "_all_hours"
        output_file = f"{symbol}_{start_date}_{end_date}_1min{hours_suffix}.csv"

    # Save to CSV
    df.to_csv(output_file, index=False)
    cprint.ok(f"Data saved to {output_file}")
    cprint.ok(f"Total bars: {len(df)}")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Save historical 1-minute bar data from alpaca-py to a csv file"
    )
    parser.add_argument("symbol", help="Stock symbol (e.g., AAPL)")
    parser.add_argument("start_date", help="Start date (YYYY-MM-DD)")
    parser.add_argument("end_date", help="End date (YYYY-MM-DD)")
    parser.add_argument("--output", help="Output CSV filename")
    parser.add_argument(
        "--include-extended-hours",
        action="store_true",
        help="Include pre-market and after-hours data (default: market hours only)",
    )

    args = parser.parse_args()

    ALPACA_API_KEY = os.getenv("ALPACA_API_KEY")
    ALPACA_API_SECRET = os.getenv("ALPACA_API_SECRET")

    if not ALPACA_API_KEY or not ALPACA_API_SECRET:
        cprint.fatal(
            "Error: Alpaca API credentials required. Set ALPACA_API_KEY and ALPACA_SECRET_KEY environment variables or use --api-key and --secret-key flags.",
            interrupt=True,
        )

    market_hours_only = not args.include_extended_hours

    try:
        fetch_historical_data(
            args.symbol,
            args.start_date,
            args.end_date,
            ALPACA_API_KEY,
            ALPACA_API_SECRET,
            args.output,
            market_hours_only,
        )
    except Exception as e:
        cprint.fatal(f"Error: {e}", interrupt=True)


if __name__ == "__main__":
    main()
