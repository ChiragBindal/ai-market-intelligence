#pragma once

#include "Trade.h"

class DeltaEngine {
private:
    double buyVolume;
    double sellVolume;
    double delta;

public:
    DeltaEngine();
    void processTrade(const Trade& trade);
    double getBuyVolume() const;
    double getSellVolume() const;
    double getDelta() const;
    void reset();
};
