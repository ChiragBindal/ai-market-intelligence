#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
EXCHANGE="$ROOT/../exchange"
ORDERFLOW_TESTS="$ROOT/../orderflow/tests"

INCLUDES=(
  -I"$ROOT/VolumeProfile/InitialBalance"
  -I"$ROOT/tests"
  -I"$ORDERFLOW_TESTS"
  -I"$EXCHANGE/include/models"
  -I"$EXCHANGE/include/enums"
  -I"$EXCHANGE/include/utility"
)

SOURCES=(
  "$EXCHANGE/src/Trade.cpp"
  "$ROOT/VolumeProfile/InitialBalance/InitialBalanceEngine.cpp"
  "$ROOT/tests/main.cpp"
  "$ROOT/tests/InitialBalanceEngineTest.cpp"
)

g++ -std=c++17 "${INCLUDES[@]}" "${SOURCES[@]}" -o "$ROOT/market_context_tests"
"$ROOT/market_context_tests"
