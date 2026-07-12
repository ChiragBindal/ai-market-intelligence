#include "FootPrintLevel.h"

FootPrintLevel::FootPrintLevel() : buyVolume(0), sellVolume(0), delta(0) {}

void FootPrintLevel::processTrade(const Trade& trade) {
    if(trade.getAggressorSide() == Side::BUY) {
        buyVolume += trade.getQuantity();
        delta += trade.getQuantity();
    } else {
        sellVolume += trade.getQuantity();
        delta -= trade.getQuantity();
    }
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