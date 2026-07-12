#include "FootPrintEngine.h"

FootPrintEngine::FootPrintEngine() : footPrintLevels() {}

void FootPrintEngine::processTrade(const Trade& trade) {
    auto& level = footPrintLevels.try_emplace(trade.getPrice()).first->second;
    level.processTrade(trade);
}

const FootPrintLevel* FootPrintEngine::getLevel(double price) const {
    auto it = footPrintLevels.find(price);
    if (it == footPrintLevels.end()) {
        return nullptr;
    }
    return &it->second;
}

void FootPrintEngine::reset() {
    footPrintLevels.clear();
}

const FootPrintLevel* FootPrintEngine::getPrevLevel(double price) const {
    auto it = footPrintLevels.find(price);
    if (it == footPrintLevels.end()) {
        return nullptr;
    }
    if (it == footPrintLevels.begin()) {
        return nullptr;
    }
    auto prev = std::prev(it);
    return &prev->second;
}

const FootPrintLevel* FootPrintEngine::getNextLevel(double price) const {
    auto it = footPrintLevels.find(price);
    if (it == footPrintLevels.end()) {
        return nullptr;
    }
    auto next = std::next(it);
    if(next == footPrintLevels.end()) return nullptr;
    return &next->second;
}