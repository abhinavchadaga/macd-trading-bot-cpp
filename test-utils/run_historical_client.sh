#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

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

echo "Creating virtual environment..."
rm -rf venv
python3 -m venv venv
# shellcheck disable=SC1091
source venv/bin/activate
pip3 install -r requirements.txt

CSV_EXISTS=false
for file in *.csv; do
	if [ -f "$file" ]; then
		CSV_EXISTS=true
		CSV_FILE="$file"
		break
	fi
done

if [ "$CSV_EXISTS" = false ]; then
	echo "No CSV file found, generating CSV data for PLTR..."
	python3 HistoricalBarsToCsv.py PLTR 2025-05-19 2025-05-23 --output PLTR_2025-05-19_2025-05-23_1min_market_hours.csv
	CSV_FILE="PLTR_2025-05-19_2025-05-23_1min_market_hours.csv"
else
	echo "CSV file already exists: $CSV_FILE, skipping generation..."
fi

echo "Starting historical WebSocket endpoint..."
python3 HistoricalAlpacaWSEndpoint.py "$CSV_FILE" --delay 0.1 &
PYTHON_PID=$!

echo "Python process started with PID: $PYTHON_PID"
wait "$PYTHON_PID"
