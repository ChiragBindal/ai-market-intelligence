#include "VolumeProfileEngine.h"
VolumeProfileEngine::VolumeProfileEngine(double valueAreaPercent) : valueAreaPercent(valueAreaPercent) {}
void VolumeProfileEngine::processTrade(const Trade& trade) {
    volumeProfileLevels[trade.getPrice()].processTrade(trade);
    totalVolume += trade.getQuantity();
    dirty = true;
}

std::map<Price, VolumeProfileLevel> VolumeProfileEngine::getVolumeProfile(){
    return volumeProfileLevels;
}

Price VolumeProfileEngine::getPointOfControl(){
    recomputeProfile();
    return pointOfControl;
}

Price VolumeProfileEngine::getValueAreaHigh() {
    recomputeProfile();
    return valueAreaHigh;
}

Price VolumeProfileEngine::getValueAreaLow(){
    recomputeProfile();
    return valueAreaLow;
}

long long VolumeProfileEngine::getTotalVolume(){
    return totalVolume;
}

void VolumeProfileEngine::reset() {
    volumeProfileLevels.clear();
    pointOfControl = 0.0;
    valueAreaHigh = 0.0;
    valueAreaLow = 0.0;
    totalVolume = 0;
    dirty = true;
}

void VolumeProfileEngine::recomputeProfile() {
    if(!dirty) {
        return;
    } 
    dirty = false;
    computePOC();
    computeValueArea();
}

void VolumeProfileEngine::computePOC() {
    long long maxVolume = 0;
    pointOfControl = 0.0;
    if (volumeProfileLevels.empty()) {
        return;
    }
    for (const auto& [price, level] : volumeProfileLevels) {
        if (level.getTotalVolume() > maxVolume) {
            maxVolume = level.getTotalVolume();
            pointOfControl = price;
        }
    }
}

void VolumeProfileEngine::computeValueArea() {
    if (volumeProfileLevels.empty()) {
        valueAreaHigh = 0.0;
        valueAreaLow = 0.0;
        return;
    }
    const long long targetVolume = static_cast<long long>(totalVolume * valueAreaPercent);
    auto pocIt = volumeProfileLevels.find(pointOfControl);
    if (pocIt == volumeProfileLevels.end()) {
        pocIt = volumeProfileLevels.begin();
    }

    auto highIt = pocIt;
    auto lowIt = pocIt;
    long long includedVolume = pocIt->second.getTotalVolume();

    valueAreaHigh = highIt->first;
    valueAreaLow = lowIt->first;

    while (includedVolume < targetVolume) {
        auto upIt = highIt;
        ++upIt;
        auto downIt = lowIt;
        if (downIt != volumeProfileLevels.begin()) {
            --downIt;
        } else {
            downIt = volumeProfileLevels.end();
        }

        const bool hasUp = upIt != volumeProfileLevels.end();
        const bool hasDown = downIt != volumeProfileLevels.end();

        if (!hasUp && !hasDown) {
            break;
        }

        const long long volUp = hasUp ? upIt->second.getTotalVolume() : -1;
        const long long volDown = hasDown ? downIt->second.getTotalVolume() : -1;

        if (!hasDown || (hasUp && volUp > volDown)) {
            includedVolume += volUp;
            highIt = upIt;
            valueAreaHigh = highIt->first;
        } else if (!hasUp || volDown > volUp) {
            includedVolume += volDown;
            lowIt = downIt;
            valueAreaLow = lowIt->first;
        } else {
            includedVolume += volUp + volDown;
            highIt = upIt;
            lowIt = downIt;
            valueAreaHigh = highIt->first;
            valueAreaLow = lowIt->first;
        }
    }
}