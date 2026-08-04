#!/usr/bin/env bash
set -eu

echo "=== ATHENA adapter tests ==="
echo "Working directory: $(pwd)"

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMMON="$ROOT/common"
EXCHANGE="$ROOT/exchange"
ORDERFLOW="$ROOT/orderflow"
ADAPTERS="$COMMON/adapters"

GPP="${ATHENA_GXX:-}"
if [[ -z "$GPP" ]]; then
  if [[ -x "/c/MinGW/bin/g++.exe" ]]; then
    GPP="/c/MinGW/bin/g++.exe"
  elif command -v g++ >/dev/null 2>&1; then
    GPP="$(command -v g++)"
  else
    echo "ERROR: g++ not found. Set ATHENA_GXX=/path/to/g++"
    exit 1
  fi
fi
echo "Using g++: $GPP"
"$GPP" --version
echo ""

INCLUDES=(
  -I"$ADAPTERS"
  -I"$ADAPTERS/tests"
  -I"$COMMON/event"
  -I"$COMMON/engine"
  -I"$COMMON/types"
  -I"$EXCHANGE/include/models"
  -I"$EXCHANGE/include/enums"
  -I"$EXCHANGE/include/utility"
  -I"$ORDERFLOW/include"
)

SOURCES=(
  "$ADAPTERS/tests/main.cpp"
  "$ADAPTERS/tests/SimulatedAdapterTest.cpp"
  "$ADAPTERS/tests/BinanceAdapterTest.cpp"
  "$ADAPTERS/MarketDataAdapter.cpp"
  "$ADAPTERS/SimulatedAdapter.cpp"
  "$ADAPTERS/BinanceAdapter.cpp"
  "$ADAPTERS/BinanceJsonParser.cpp"
  "$ADAPTERS/BinanceHttpClient.cpp"
  "$ADAPTERS/BinanceWebSocketClient.cpp"
  "$ADAPTERS/LiveExchangeAdapters.cpp"
  "$COMMON/event/EventDispatcher.cpp"
  "$COMMON/event/TradeEvent.cpp"
  "$EXCHANGE/src/Trade.cpp"
  "$ORDERFLOW/src/DeltaEngine.cpp"
  "$ORDERFLOW/src/FootPrintLevel.cpp"
  "$ORDERFLOW/src/FootPrintEngine.cpp"
)

OUT_BASE="$ADAPTERS/adapter_tests"
rm -f "$OUT_BASE" "$OUT_BASE.exe" 2>/dev/null || true

echo "Compiling -> $OUT_BASE"
if ! "$GPP" -std=c++17 "${INCLUDES[@]}" "${SOURCES[@]}" -o "$OUT_BASE"; then
  echo ""
  echo "COMPILE FAILED - read g++ errors above."
  exit 1
fi

BIN=""
if [[ -x "${OUT_BASE}.exe" ]]; then
  BIN="${OUT_BASE}.exe"
elif [[ -x "$OUT_BASE" ]]; then
  BIN="$OUT_BASE"
else
  echo "ERROR: binary not found after compile"
  exit 1
fi

echo "Compile OK. Running: $BIN"
"$BIN"
echo "=== Done (tests) ==="
