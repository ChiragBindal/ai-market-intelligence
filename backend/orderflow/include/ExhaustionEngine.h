#pragma once

#include <deque>
#include "Side.h"

class ExhaustionEngine {
private:
    std::deque<double> buyVolumes;
    std::deque<double> sellVolumes;

    size_t rollingWindowSize;
    double minimumPercentageDecrease;
    size_t minimumConsecutiveDeclines;

    bool isExhausted(const std::deque<double>& volumes) const;

public:
    ExhaustionEngine(size_t rollingWindowSize = 10, double minimumPercentageDecrease = 0.01, size_t minimumConsecutiveDeclines = 3);
    void processObservation(Side side, double volume);
    bool isBuyExhaustion() const;
    bool isSellExhaustion() const;
    const std::deque<double>& getBuyVolumes() const;
    const std::deque<double>& getSellVolumes() const;
    void reset();
};
