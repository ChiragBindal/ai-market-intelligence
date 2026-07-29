#pragma once
#include "Trade.h"

class VolumeProfileLevel {
    private:
    long long totalVolume = 0;

    public:
    void processTrade(const Trade& trade);
    long long getTotalVolume() const;
    void reset();
};