#include "ExhaustionEngine.h"

ExhaustionEngine::ExhaustionEngine(
    size_t rollingWindowSize,
    double minimumPercentageDecrease,
    size_t minimumConsecutiveDeclines)
    : rollingWindowSize(rollingWindowSize),
      minimumPercentageDecrease(minimumPercentageDecrease),
      minimumConsecutiveDeclines(minimumConsecutiveDeclines) {}

void ExhaustionEngine::processObservation(Side side, double volume) {
    if (volume <= 0) {
        return;
    }
    auto& volumes = (side == Side::BUY) ? buyVolumes : sellVolumes;
    volumes.push_back(volume);
    if (volumes.size() > rollingWindowSize) {
        volumes.pop_front();
    }
}

bool ExhaustionEngine::isExhausted(const std::deque<double>& volumes) const {
    if (volumes.size() < rollingWindowSize) {
        return false;
    }
    size_t consecutiveDeclines = 0;
    for (size_t i = 1; i < volumes.size(); ++i) {
        double previous = volumes[i - 1];
        double current = volumes[i];
        double percentageDecrease =(previous - current) / previous;
        if (percentageDecrease >= minimumPercentageDecrease) {
            ++consecutiveDeclines;
            if (consecutiveDeclines >= minimumConsecutiveDeclines) {
                return true;
            }
        } else {
            consecutiveDeclines = 0;
        }
    }
    return false;
}

bool ExhaustionEngine::isBuyExhaustion() const {
    return isExhausted(buyVolumes);
}

bool ExhaustionEngine::isSellExhaustion() const {
    return isExhausted(sellVolumes);
}

void ExhaustionEngine::reset() {
    buyVolumes.clear();
    sellVolumes.clear();
}

const std::deque<double>& ExhaustionEngine::getBuyVolumes() const {
    return buyVolumes;
}

const std::deque<double>& ExhaustionEngine::getSellVolumes() const {
    return sellVolumes;
}