#!/bin/bash
set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

WEBSOCAT_VERSION="v1.14.0"
CERT_DIR="/tmp/test_certs"
PID_FILE="/tmp/websocat_test.pid"
PORT="9001"

log_info() {
	echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
	echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
	echo -e "${RED}[ERROR]${NC} $1"
}

cleanup() {
	if [ -f "$PID_FILE" ]; then
		local pid
		pid=$(cat "$PID_FILE")
		if kill -0 "$pid" 2>/dev/null; then
			log_info "Stopping websocat server (PID: $pid)"
			kill "$pid"
			rm -f "$PID_FILE"
		fi
	fi
}

install_websocat() {
	if command -v websocat &>/dev/null; then
		log_info "websocat already installed: $(websocat --version)"
		return 0
	fi

	log_info "Installing websocat $WEBSOCAT_VERSION..."

	local os
	local arch
	os=$(uname -s)
	arch=$(uname -m)
	local package_name=""

	case "$os-$arch" in
	"Darwin-arm64")
		package_name="websocat.aarch64-apple-darwin"
		;;
	"Darwin-x86_64")
		package_name="websocat.x86_64-apple-darwin"
		;;
	"Linux-x86_64")
		package_name="websocat.x86_64-unknown-linux-musl"
		;;
	"Linux-aarch64")
		package_name="websocat_max.aarch64-unknown-linux-musl"
		;;
	*)
		log_error "Unsupported platform: $os-$arch"
		exit 1
		;;
	esac

	local url="https://github.com/vi/websocat/releases/download/${WEBSOCAT_VERSION}/${package_name}"
	local temp_file="/tmp/websocat"

	if ! curl -L -o "$temp_file" "$url"; then
		log_error "Failed to download websocat"
		exit 1
	fi

	chmod +x "$temp_file"

	if sudo mv "$temp_file" /usr/local/bin/websocat 2>/dev/null; then
		log_info "websocat installed to /usr/local/bin/websocat"
	elif mv "$temp_file" ./websocat 2>/dev/null; then
		log_info "websocat installed to ./websocat (local directory)"
		export PATH=".:$PATH"
	else
		log_error "Failed to install websocat"
		exit 1
	fi
}

generate_certificates() {
	log_info "Generating test certificates in $CERT_DIR"

	mkdir -p "$CERT_DIR"

	# Generate private key
	openssl genrsa -out "$CERT_DIR/key.pem" 2048 2>/dev/null

	# Generate certificate
	openssl req -new -x509 -key "$CERT_DIR/key.pem" -out "$CERT_DIR/cert.pem" -days 1 \
		-subj "/C=US/ST=Test/L=Test/O=Test/CN=localhost" 2>/dev/null

	# Create PKCS12 bundle
	openssl pkcs12 -export -out "$CERT_DIR/echo.p12" \
		-inkey "$CERT_DIR/key.pem" -in "$CERT_DIR/cert.pem" \
		-passout pass:pass123 2>/dev/null

	log_info "Certificates generated successfully"
}

start_server() {
	log_info "Starting websocat echo server on port $PORT"

	# Start websocat in background
	websocat -E --text \
		--pkcs12-der="$CERT_DIR/echo.p12" \
		--pkcs12-passwd=pass123 \
		"wss-l:127.0.0.1:$PORT" mirror: &

	local pid
	pid=$!
	echo "$pid" >"$PID_FILE"

	log_info "websocat server started with PID: $pid"

	# Wait for server to be ready
	log_info "Waiting for server to be ready..."
	sleep 3

	# Verify server is running
	if ! kill -0 "$pid" 2>/dev/null; then
		log_error "websocat server failed to start"
		exit 1
	fi

	log_info "websocat echo server is ready on wss://127.0.0.1:$PORT"
}

stop_server() {
	cleanup
	log_info "websocat server stopped"
}

main() {
	case "${1:-start}" in
	"start")
		cleanup # Clean up any existing server
		install_websocat
		generate_certificates
		start_server
		;;
	"stop")
		stop_server
		;;
	"restart")
		stop_server
		sleep 1
		main start
		;;
	*)
		echo "Usage: $0 [start|stop|restart]"
		exit 1
		;;
	esac
}

# Set up signal handlers for cleanup - but only for stop/restart, not start
case "${1:-start}" in
"start")
	# Don't set up EXIT trap for start command - let server run
	trap cleanup INT TERM
	;;
*)
	trap cleanup EXIT INT TERM
	;;
esac

main "$@"
