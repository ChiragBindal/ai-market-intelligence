#include "AbsorptionEngine.h"

AbsorptionEngine::AbsorptionEngine(const FootPrintEngine& footprint, const ImbalanceEngine& imbalance, double volumeThreshold)
    : m_footprint(footprint), m_imbalance(imbalance), volumeThreshold(volumeThreshold) {}

// Suppose at next price level we only have 5 share , so is it price move ? , no so we need to introduce threshold
bool AbsorptionEngine::isSellAbsorption(double price) const {
    const FootPrintLevel* currLevel = m_footprint.getLevel(price);
    if(currLevel == nullptr) {
        return false;
    }
    const FootPrintLevel* next = m_footprint.getNextLevel(price);
    if(next != nullptr) {
        return false;
    }
    return m_imbalance.isBuyImbalance(price) && currLevel->getBuyVolume() > volumeThreshold;
}

bool AbsorptionEngine::isBuyAbsorption(double price) const {
    const FootPrintLevel* currLevel = m_footprint.getLevel(price);
    if(currLevel == nullptr) {
        return false;
    }
    if (m_footprint.getPrevLevel(price) != nullptr) return false;
    return m_imbalance.isSellImbalance(price) && currLevel->getSellVolume() > volumeThreshold;
}