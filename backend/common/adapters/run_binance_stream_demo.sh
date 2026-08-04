#!/usr/bin/env bash
set -eu

echo "=== ATHENA Binance WebSocket + FootPrint demo ==="
echo "Working directory: $(pwd)"

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMMON="$ROOT/common"
EXCHANGE="$ROOT/exchange"
ORDERFLOW="$ROOT/orderflow"
ADAPTERS="$COMMON/adapters"

pick_gpp() {
  if [[ -n "${ATHENA_GXX:-}" ]]; then
    echo "$ATHENA_GXX"
    return
  fi
  if [[ -x "/c/MinGW/bin/g++.exe" ]]; then
    echo "/c/MinGW/bin/g++.exe"
    return
  fi
  if command -v g++ >/dev/null 2>&1; then
    command -v g++
    return
  fi
  echo "ERROR: no g++ found. Set ATHENA_GXX=/path/to/g++.exe" >&2
  exit 1
}

GPP="$(pick_gpp)"
echo "Using g++: $GPP"
"$GPP" --version || true
echo ""

INCLUDES=(
  -I"$ADAPTERS"
  -I"$COMMON/event"
  -I"$COMMON/engine"
  -I"$COMMON/types"
  -I"$EXCHANGE/include/models"
  -I"$EXCHANGE/include/enums"
  -I"$EXCHANGE/include/utility"
  -I"$ORDERFLOW/include"
)

SOURCES=(
  "$ADAPTERS/demo/binance_stream_main.cpp"
  "$ADAPTERS/MarketDataAdapter.cpp"
  "$ADAPTERS/BinanceAdapter.cpp"
  "$ADAPTERS/BinanceJsonParser.cpp"
  "$ADAPTERS/BinanceHttpClient.cpp"
  "$ADAPTERS/BinanceWebSocketClient.cpp"
  "$COMMON/event/EventDispatcher.cpp"
  "$COMMON/event/TradeEvent.cpp"
  "$EXCHANGE/src/Trade.cpp"
  "$ORDERFLOW/src/DeltaEngine.cpp"
  "$ORDERFLOW/src/FootPrintLevel.cpp"
  "$ORDERFLOW/src/FootPrintEngine.cpp"
)

OUT_BASE="$ADAPTERS/binance_stream_demo"
rm -f "$OUT_BASE" "$OUT_BASE.exe" 2>/dev/null || true

echo "Compiling -> $OUT_BASE"
if ! "$GPP" -std=c++17 "${INCLUDES[@]}" "${SOURCES[@]}" -o "$OUT_BASE"; then
  echo "COMPILE FAILED"
  exit 1
fi

if [[ -x "${OUT_BASE}.exe" ]]; then BIN="${OUT_BASE}.exe"; else BIN="$OUT_BASE"; fi

MODE="${1:-stream}"
SECONDS_ARG="${2:-15}"
MAX_TRADES="${3:-80}"

echo "Compile OK. Running: $BIN $MODE $SECONDS_ARG $MAX_TRADES"
echo "(WebSocket needs PowerShell + internet; REST fallback if stream empty)"
"$BIN" "$MODE" "$SECONDS_ARG" "$MAX_TRADES"
echo "=== Done (binance stream demo) ==="
