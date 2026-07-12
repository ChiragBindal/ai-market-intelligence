#include "ImbalanceEngine.h"

ImbalanceEngine::ImbalanceEngine(const FootPrintEngine& footPrintEngine, double imbalanceRatio)
    : footPrintEngine(footPrintEngine), imbalanceRatio(imbalanceRatio) {}

bool ImbalanceEngine::isBuyImbalance(double price) const {
    const FootPrintLevel* currLevel = footPrintEngine.getLevel(price);
    if(currLevel == nullptr) {
        return false;
    }
    const FootPrintLevel* nextLevel = footPrintEngine.getNextLevel(price);
    if(nextLevel == nullptr) {
        return false;
    }
    return currLevel->getBuyVolume() > imbalanceRatio * nextLevel->getSellVolume();
}
bool ImbalanceEngine::isSellImbalance(double price) const {
    const FootPrintLevel* currLevel = footPrintEngine.getLevel(price);
    if(currLevel == nullptr) {
        return false;
    }
    const FootPrintLevel* prevLevel = footPrintEngine.getPrevLevel(price);
    if(prevLevel == nullptr) {
        return false;
    }
    return currLevel->getSellVolume() > imbalanceRatio * prevLevel->getBuyVolume();
}