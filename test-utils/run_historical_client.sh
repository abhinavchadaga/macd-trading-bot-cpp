#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

# Parse command line arguments
SYMBOL="${1:-PLTR}"
START_DATE="${2:-2025-05-19}"
END_DATE="${3:-2025-05-23}"

if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
	echo "Usage: $0 [symbol] [start_date] [end_date]"
	echo "  symbol:     Stock symbol (default: PLTR)"
	echo "  start_date: Start date in YYYY-MM-DD format (default: 2025-05-19)"
	echo "  end_date:   End date in YYYY-MM-DD format (default: 2025-05-23)"
	exit 0
fi

PYTHON_PID=""

cleanup() {
	echo "Cleanup called, terminating Python process..."
	if [ -n "$PYTHON_PID" ] && kill -0 "$PYTHON_PID" 2>/dev/null; then
		echo "Sending SIGINT to Python process $PYTHON_PID"
		kill -INT "$PYTHON_PID"
		sleep 2
		if kill -0 "$PYTHON_PID" 2>/dev/null; then
			echo "Process still running, sending SIGTERM"
			kill -TERM "$PYTHON_PID"
			sleep 1
			if kill -0 "$PYTHON_PID" 2>/dev/null; then
				echo "Force killing process"
				kill -KILL "$PYTHON_PID"
			fi
		fi
		wait "$PYTHON_PID" 2>/dev/null || true
	fi
	exit 0
}

trap cleanup SIGTERM SIGINT

echo "Checking if port 8765 is in use..."
PORT_PID=$(lsof -ti:8765 2>/dev/null || true)
if [ -n "$PORT_PID" ]; then
	echo "Port 8765 is in use by process $PORT_PID, killing it..."
	kill -9 "$PORT_PID" 2>/dev/null || true
	sleep 1
fi

historical_bars_to_csv="build/python-utils/historical_bars_to_csv"
historical_alpaca_ws_endpoint="build/python-utils/historical_alpaca_ws_endpoint"

CSV_FILE="/tmp/${SYMBOL}_${START_DATE}_${END_DATE}_1min_market_hours.csv"
./"$historical_bars_to_csv" "$SYMBOL" "$START_DATE" "$END_DATE" --output "${CSV_FILE}"

echo "Starting historical WebSocket endpoint..."
./"$historical_alpaca_ws_endpoint" "$CSV_FILE" --delay 0.1 &
PYTHON_PID=$!

echo "Python process started with PID: $PYTHON_PID"
wait "$PYTHON_PID"
