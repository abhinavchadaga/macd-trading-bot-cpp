#!/usr/bin/env python3
import asyncio
import csv
import json
import logging
import signal
import ssl
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Optional, cast

from websockets.asyncio.server import ServerConnection, serve
from cprint import cprint


logger = logging.getLogger("HistoricalAlpacaWSEndpoint")


class HistoricalAlpacaWSEndpoint:
    """WebSocket server for streaming historical bar data from CSV."""

    def __init__(self, csv_path: str, delay_seconds: float = 1.0):
        """
        Initialize the historical data endpoint.

        Args:
            csv_path: Path to CSV file with historical bar data
            delay_seconds: Delay between bar emissions
        """
        self._csv_path = Path(csv_path)
        self._delay_seconds = delay_seconds
        self._bars: list[dict] = []

        logging.basicConfig(
            level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
        )
        self._load_bars()
        self._symbol = self._bars[0]["S"]
        self._ssl_context = self._create_ssl_context()

    def _create_ssl_context(self) -> ssl.SSLContext:
        """Create SSL context for secure WebSocket connections."""
        ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        ssl_context.check_hostname = False
        ssl_context.verify_mode = ssl.CERT_NONE

        cert_file = Path(__file__).parent / "server.crt"
        key_file = Path(__file__).parent / "server.key"

        if not cert_file.exists() or not key_file.exists():
            logger.info("Generating self-signed certificate...")
            self._generate_self_signed_cert(cert_file, key_file)

        ssl_context.load_cert_chain(cert_file, key_file)
        return ssl_context

    def _generate_self_signed_cert(self, cert_file: Path, key_file: Path) -> None:
        """Generate self-signed certificate for testing."""
        import subprocess

        try:
            subprocess.run(
                [
                    "openssl",
                    "req",
                    "-x509",
                    "-newkey",
                    "rsa:4096",
                    "-keyout",
                    str(key_file),
                    "-out",
                    str(cert_file),
                    "-days",
                    "365",
                    "-nodes",
                    "-subj",
                    "/C=US/ST=State/L=City/O=Organization/CN=localhost",
                ],
                check=True,
                capture_output=True,
            )
            logger.info(f"Generated certificate: {cert_file}")
            logger.info(f"Generated key: {key_file}")
        except subprocess.CalledProcessError as e:
            logger.error(f"Failed to generate certificate: {e}")
            raise

    def _load_bars(self) -> None:
        """Load bar data from CSV file."""
        if not self._csv_path.exists():
            raise FileNotFoundError(f"CSV file not found: {self._csv_path}")

        logger.info(f"Loading bars from {self._csv_path}")

        with open(self._csv_path, "r") as f:
            reader = csv.DictReader(f)
            for row in reader:
                bar = self._convert_csv_row_to_alpaca_format(row)
                self._bars.append(bar)

        logger.info(f"Loaded {len(self._bars)} bars")

    def _convert_csv_row_to_alpaca_format(
        self, row: dict[str, str]
    ) -> dict[str, str | float]:
        """Convert CSV row to Alpaca WebSocket bar format."""
        timestamp_str = row["timestamp"]
        timestamp_rfc3339: Optional[str] = None
        try:
            timestamp_iso = timestamp_str.replace(" ", "T")
            timestamp_dt = datetime.fromisoformat(timestamp_iso)
            timestamp_rfc3339 = timestamp_dt.astimezone(timezone.utc).isoformat(
                timespec="seconds"
            )
            timestamp_rfc3339 = timestamp_rfc3339.replace("+00:00", "Z")
        except Exception as e:
            logger.error(f"Failed to parse timestamp '{timestamp_str}': {e}")

        return {
            "T": "b",
            "S": row["symbol"],
            "o": float(row["open"]),
            "h": float(row["high"]),
            "l": float(row["low"]),
            "c": float(row["close"]),
            "v": float(row["volume"]),
            "t": cast(str, timestamp_rfc3339),
            "n": float(row["trade_count"]),
            "vw": float(row["vwap"]),
        }

    async def _perform_alpaca_handshake(self, websocket: ServerConnection):
        # send on connect message
        cprint.info("Performing alpaca handshake")
        successfully_connected_msg = json.dumps([{"T": "success", "msg": "connected"}])
        await websocket.send(message=successfully_connected_msg)

        # wait for auth msg
        auth_msg = await websocket.recv(decode=True)
        auth_msg = json.loads(auth_msg)
        cprint.info(f"Received auth message: '{auth_msg}'")
        assert all(k in auth_msg for k in ["action", "key", "secret"])

        # send authorized msg back
        successfully_authorized_msg = json.dumps(
            obj=[{"T": "success", "msg": "authenticated"}]
        )
        await websocket.send(message=successfully_authorized_msg)

        # wait for subscription message
        subscribe_msg = await websocket.recv(decode=True)
        subscribe_msg = json.loads(subscribe_msg)
        cprint.info(f"Received subscribe message: '{subscribe_msg}'")
        assert (
            subscribe_msg["action"] == "subscribe"
            and self._symbol in subscribe_msg["bars"]
        )

        # send confirmation of subscription
        subscribed_msg = [
            {
                "T": "subscription",
                "trades": [],
                "quotes": [],
                "bars": [self._symbol],
                "updatedBars": [],
                "dailyBars": [],
                "statuses": [],
                "lulds": [],
                "corrections": [],
                "cancelErrors": [],
            }
        ]
        subscribed_msg = json.dumps(subscribed_msg)
        await websocket.send(subscribed_msg)

    async def _stream_bars(self, websocket: ServerConnection):
        try:
            for bar in self._bars:
                msg = [bar]
                await websocket.send(json.dumps(msg))
                await asyncio.sleep(delay=self._delay_seconds)
        except Exception as e:
            logger.warning(f"Client disconnected during streaming: {e}")
            return

    async def run(self, websocket: ServerConnection):
        await self._perform_alpaca_handshake(websocket)
        await self._stream_bars(websocket)

    async def main(self):
        async with serve(
            self.run, "127.0.0.1", port=8765, ssl=self._ssl_context
        ) as server:
            loop = asyncio.get_event_loop()
            loop.add_signal_handler(signal.SIGTERM, server.close)
            await server.wait_closed()


def main():
    """Main entry point."""
    import argparse

    parser = argparse.ArgumentParser(description="Historical Alpaca WebSocket Endpoint")
    parser.add_argument("csv_path", help="Path to CSV file with historical bar data")
    parser.add_argument(
        "--delay",
        type=float,
        default=1.0,
        help="Delay in seconds between bar emissions (default: 1.0)",
    )

    args = parser.parse_args()

    try:
        endpoint = HistoricalAlpacaWSEndpoint(args.csv_path, args.delay)
        asyncio.run(endpoint.main())
    except FileNotFoundError as e:
        logger.error(f"Error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        logger.info("Server stopped.")
    except Exception as e:
        logger.error(f"Error: {e}")
        sys.exit(1)
    finally:
        cert_file = Path(__file__).parent / "server.crt"
        key_file = Path(__file__).parent / "server.key"
        if cert_file.exists():
            cert_file.unlink()
            logger.info(f"Cleaned up certificate: {cert_file}")
        if key_file.exists():
            key_file.unlink()
            logger.info(f"Cleaned up key: {key_file}")

        logger.info("Graceful shutdown complete.")


if __name__ == "__main__":
    main()
