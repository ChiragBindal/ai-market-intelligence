#!/usr/bin/env bash
# Run this if compile fails: shows step-by-step what breaks
set -eu

ADAPTERS="$(cd "$(dirname "$0")" && pwd)"
COMMON="$(cd "$ADAPTERS/.." && pwd)"
EXCHANGE="$(cd "$ADAPTERS/../../exchange" && pwd)"
ORDERFLOW="$(cd "$ADAPTERS/../../orderflow" && pwd)"

GPP="${ATHENA_GXX:-g++}"
echo "Diagnose compile with: $GPP"
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

try_compile() {
  local src="$1"
  local out="$2"
  echo ">> compiling $src"
  if ! "$GPP" -std=c++17 "${INCLUDES[@]}" -c "$src" -o "$out"; then
    echo "FAILED on $src"
    exit 1
  fi
}

rm -f "$ADAPTERS"/diag_*.o 2>/dev/null || true

try_compile "$ADAPTERS/demo/binance_main.cpp" "$ADAPTERS/diag_main.o"
try_compile "$ADAPTERS/BinanceJsonParser.cpp" "$ADAPTERS/diag_parser.o"
try_compile "$ADAPTERS/BinanceHttpClient.cpp" "$ADAPTERS/diag_http.o"
try_compile "$ADAPTERS/BinanceAdapter.cpp" "$ADAPTERS/diag_adapter.o"
try_compile "$COMMON/event/EventDispatcher.cpp" "$ADAPTERS/diag_disp.o"
try_compile "$EXCHANGE/src/Trade.cpp" "$ADAPTERS/diag_trade.o"
try_compile "$ORDERFLOW/src/DeltaEngine.cpp" "$ADAPTERS/diag_delta.o"

echo ""
echo "All single-file compiles OK."
echo "If full link still fails, run: ./run_binance_demo.sh"
echo "Or force MinGW: export ATHENA_GXX=/c/MinGW/bin/g++.exe"
