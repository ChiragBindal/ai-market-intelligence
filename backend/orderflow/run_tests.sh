#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
EXCHANGE="$ROOT/../exchange"

INCLUDES=(
  -I"$ROOT/include"
  -I"$ROOT/tests"
  -I"$EXCHANGE/include/models"
  -I"$EXCHANGE/include/enums"
  -I"$EXCHANGE/include/utility"
)

SOURCES=(
  "$EXCHANGE/src/Trade.cpp"
  "$ROOT/src/DeltaEngine.cpp"
  "$ROOT/src/FootPrintLevel.cpp"
  "$ROOT/src/FootPrintEngine.cpp"
  "$ROOT/src/ExhaustionEngine.cpp"
  "$ROOT/tests/main.cpp"
  "$ROOT/tests/DeltaEngineTest.cpp"
  "$ROOT/tests/FootPrintLevelTest.cpp"
  "$ROOT/tests/FootPrintEngineTest.cpp"
  "$ROOT/tests/ExhaustionEngineTest.cpp"
)

g++ -std=c++17 "${INCLUDES[@]}" "${SOURCES[@]}" -o "$ROOT/orderflow_tests"
"$ROOT/orderflow_tests"
