#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

INCLUDES=(
  -I"$ROOT/common/event"
  -I"$ROOT/common/engine"
  -I"$ROOT/common/types"
  -I"$ROOT/exchange/include/models"
  -I"$ROOT/exchange/include/enums"
  -I"$ROOT/exchange/include/utility"
  -I"$ROOT/orderflow/include"
)

SOURCES=(
  "$ROOT/common/event/main.cpp"
  "$ROOT/common/event/EventDispatcher.cpp"
  "$ROOT/common/event/TradeEvent.cpp"
  "$ROOT/exchange/src/Trade.cpp"
  "$ROOT/orderflow/src/DeltaEngine.cpp"
)

g++ -std=c++17 "${INCLUDES[@]}" "${SOURCES[@]}" -o "$ROOT/common/event/event_dispatcher_demo"
"$ROOT/common/event/event_dispatcher_demo"
