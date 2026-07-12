#include "DeltaEngine.h"

#include <stdexcept>

DeltaEngine::DeltaEngine() : buyVolume(0), sellVolume(0), delta(0) {}

void DeltaEngine::processTrade(const Trade& trade) {
    if (trade.getQuantity() <= 0) {
        throw std::invalid_argument("Trade quantity must be greater than 0");
    }
    if (trade.getAggressorSide() == Side::BUY) {
        buyVolume += trade.getQuantity();
        delta += trade.getQuantity();
    } else {
        sellVolume += trade.getQuantity();
        delta -= trade.getQuantity();
    }
}

double DeltaEngine::getBuyVolume() const {
    return buyVolume;
}

double DeltaEngine::getSellVolume() const {
    return sellVolume;
}

double DeltaEngine::getDelta() const {
    return delta;
}

void DeltaEngine::reset() {
    buyVolume = 0;
    sellVolume = 0;
    delta = 0;
}
