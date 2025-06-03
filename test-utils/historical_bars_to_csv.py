#!/usr/bin/env python3
import argparse
import os
from datetime import datetime
from typing import Optional

import pandas as pd
from alpaca.data import BarSet
from alpaca.data.historical import StockHistoricalDataClient
from alpaca.data.requests import StockBarsRequest
from alpaca.data.timeframe import TimeFrame, TimeFrameUnit
from cprint import cprint
from dotenv import load_dotenv

load_dotenv()

ALPACA_API_KEY = os.getenv("ALPACA_API_KEY")
ALPACA_API_SECRET = os.getenv("ALPACA_API_SECRET")

if ALPACA_API_KEY is None or ALPACA_API_KEY is None:
    raise Exception("No Alpaca keys found")


def fetch_historical_data(
    symbol: str,
    start_date: str,
    end_date: str,
    output_file: Optional[str] = None,
    market_hours_only: bool = True,
    limit: Optional[int] = None,
) -> None:
    """Fetch historical 1-minute bar data and save to CSV."""

    client = StockHistoricalDataClient(ALPACA_API_KEY, ALPACA_API_SECRET)
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

    df = bars.df
    if df.empty:
        cprint.err("No data found for the specified period.", interrupt=True)

    df = df.reset_index()
    if market_hours_only:
        cprint.info(
            "Filtering to regular market hours (9:30 AM-4 PM ET, weekdays only)…"
        )

        if "timestamp" not in df.columns:
            raise ValueError("No timestamp column found in data")

        df["timestamp"] = pd.to_datetime(df["timestamp"])
        df["timestamp_et"] = df["timestamp"].dt.tz_convert("US/Eastern")
        df = df[
            (df["timestamp_et"].dt.time >= pd.Timestamp("09:30:00").time())
            & (df["timestamp_et"].dt.time <= pd.Timestamp("16:00:00").time())
            & (df["timestamp_et"].dt.weekday < 5)
        ]
        df.drop("timestamp_et", axis=1)

    if df.empty:
        cprint.warn("No data found after filtering.")
        return

    if limit is not None and limit > 0:
        df = df.head(limit)
        cprint.info(f"Limited to first {limit} bars")

    if output_file is None:
        hours_suffix = "_market_hours" if market_hours_only else "_all_hours"
        output_file = f"{symbol}_{start_date}_{end_date}_1min{hours_suffix}.csv"

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
    parser.add_argument(
        "--limit",
        type=int,
        help="Limit the number of bars returned (useful for testing)",
    )

    args = parser.parse_args()
    market_hours_only = not args.include_extended_hours

    try:
        fetch_historical_data(
            args.symbol,
            args.start_date,
            args.end_date,
            args.output,
            market_hours_only,
            args.limit,
        )
    except Exception as e:
        cprint.fatal(f"Error: {e}", interrupt=True)


if __name__ == "__main__":
    main()
