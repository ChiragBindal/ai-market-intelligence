#include "FootPrintLevel.h"

FootPrintLevel::FootPrintLevel()
    : buyVolume(0),
      sellVolume(0),
      delta(0),
      tradeCount(0),
      maxPrintSize(0) {}

bool FootPrintLevel::processTrade(const Trade& trade) {
    if (trade.getQuantity() <= 0) {
        return false;
    }

    const double qty = trade.getQuantity();
    if (trade.getAggressorSide() == Side::BUY) {
        buyVolume += qty;
        delta += qty;
    } else {
        sellVolume += qty;
        delta -= qty;
    }

    ++tradeCount;
    if (qty > maxPrintSize) {
        maxPrintSize = qty;
    }
    return true;
}

double FootPrintLevel::getBuyVolume() const {
    return buyVolume;
}

double FootPrintLevel::getSellVolume() const {
    return sellVolume;
}

double FootPrintLevel::getDelta() const {
    return delta;
}

double FootPrintLevel::getTotalVolume() const {
    return buyVolume + sellVolume;
}

int FootPrintLevel::getTradeCount() const {
    return tradeCount;
}

double FootPrintLevel::getMaxPrintSize() const {
    return maxPrintSize;
}

void FootPrintLevel::reset() {
    buyVolume = 0;
    sellVolume = 0;
    delta = 0;
    tradeCount = 0;
    maxPrintSize = 0;
}
