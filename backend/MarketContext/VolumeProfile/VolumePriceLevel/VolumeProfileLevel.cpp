#include "VolumeProfileLevel.h"

void VolumeProfileLevel::processTrade(const Trade& trade) {
    totalVolume += trade.getQuantity();
}

long long VolumeProfileLevel::getTotalVolume() const {
    return totalVolume;
}

void VolumeProfileLevel::reset() {
    totalVolume = 0;
}