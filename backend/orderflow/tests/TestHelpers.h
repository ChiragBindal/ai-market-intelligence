#pragma once

#include "Trade.h"
#include "Side.h"

inline Trade makeTrade(Side aggressorSide, double quantity, double price = 100.0) {
    return Trade(1, 2, "AAPL", price, quantity, aggressorSide);
}

inline bool approxEqual(double a, double b, double epsilon = 1e-9) {
    return (a > b ? a - b : b - a) < epsilon;
}
