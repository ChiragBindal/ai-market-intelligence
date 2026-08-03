#pragma once

#include "Trade.h"

class FootPrintLevel {
private:
    double buyVolume;
    double sellVolume;
    double delta;
    int tradeCount;
    double maxPrintSize;

public:
    FootPrintLevel();

    // Returns false if trade quantity is invalid (skipped).
    bool processTrade(const Trade& trade);

    double getBuyVolume() const;
    double getSellVolume() const;
    double getDelta() const;
    double getTotalVolume() const;
    int getTradeCount() const;
    double getMaxPrintSize() const;
    void reset();
};
