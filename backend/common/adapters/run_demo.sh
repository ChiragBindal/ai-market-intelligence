#!/usr/bin/env bash
set -eu

echo "=== ATHENA Simulated adapter demo ==="
echo "Working directory: $(pwd)"

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMMON="$ROOT/common"
EXCHANGE="$ROOT/exchange"
ORDERFLOW="$ROOT/orderflow"
ADAPTERS="$COMMON/adapters"

GPP="${ATHENA_GXX:-g++}"
echo "Using g++: $(command -v "$GPP")"

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
  "$ADAPTERS/demo/main.cpp"
  "$ADAPTERS/MarketDataAdapter.cpp"
  "$ADAPTERS/SimulatedAdapter.cpp"
  "$ADAPTERS/LiveExchangeAdapters.cpp"
  "$COMMON/event/EventDispatcher.cpp"
  "$COMMON/event/TradeEvent.cpp"
  "$EXCHANGE/src/Trade.cpp"
  "$ORDERFLOW/src/DeltaEngine.cpp"
)

OUT_BASE="$ADAPTERS/adapter_demo"
rm -f "$OUT_BASE" "$OUT_BASE.exe" 2>/dev/null || true

echo "Compiling -> $OUT_BASE"
if ! "$GPP" -std=c++17 "${INCLUDES[@]}" "${SOURCES[@]}" -o "$OUT_BASE"; then
  echo "COMPILE FAILED"
  exit 1
fi

if [[ -x "${OUT_BASE}.exe" ]]; then BIN="${OUT_BASE}.exe"; else BIN="$OUT_BASE"; fi
echo "Compile OK. Running: $BIN"
"$BIN"
echo "=== Done (simulated demo) ==="
