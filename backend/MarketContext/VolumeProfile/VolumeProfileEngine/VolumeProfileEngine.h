#pragma once
#include "VolumeProfileLevel.h"
#include <map>
using Price = double;

class VolumeProfileEngine {
    private:
    std::map<Price, VolumeProfileLevel> volumeProfileLevels;
    Price pointOfControl = 0.0;
    Price valueAreaHigh = 0.0;
    Price valueAreaLow = 0.0;
    long long totalVolume = 0;
    bool dirty = true;
    double valueAreaPercent = 0.70;
    public:
    explicit VolumeProfileEngine(double valueAreaPercent = 0.70);
    void processTrade(const Trade& trade);
    std::map<Price, VolumeProfileLevel> getVolumeProfile();
    Price getPointOfControl();
    Price getValueAreaHigh();
    Price getValueAreaLow();
    long long getTotalVolume();
    void reset();

    private:
    void recomputeProfile();
    void computePOC();
    void computeValueArea();
    
};