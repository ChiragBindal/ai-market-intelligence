#pragma once

#include "Trade.h"
#include "Side.h"

#include <chrono>

inline Trade makeTrade(Side aggressorSide, double quantity, double price,
                       std::chrono::system_clock::time_point tradeTime) {
    return Trade(1, 2, "AAPL", price, quantity, aggressorSide, tradeTime);
}

inline std::chrono::system_clock::time_point minutesFromEpoch(int minutes) {
    return std::chrono::system_clock::time_point{std::chrono::minutes(minutes)};
}
