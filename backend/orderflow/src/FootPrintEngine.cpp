#include "FootPrintEngine.h"
#include "TradeEvent.h"

#include <cmath>

FootPrintEngine::FootPrintEngine(double tickSize, std::string symbolFilter)
    : tickSize(tickSize),
      symbolFilter(std::move(symbolFilter)) {}

FootPrintEngine::Tick FootPrintEngine::toTick(double price) const {
    return static_cast<Tick>(std::llround(price / tickSize));
}

double FootPrintEngine::toPrice(Tick tick) const {
    return static_cast<double>(tick) * tickSize;
}

std::map<FootPrintEngine::Tick, FootPrintLevel>::const_iterator
FootPrintEngine::findTick(double price) const {
    return footPrintLevels.find(toTick(price));
}

void FootPrintEngine::onEvent(const MarketEvent& event) {
    if (event.type() != EventType::Trade) {
        return;
    }
    const auto& tradeEvent = static_cast<const TradeEvent&>(event);
    processTrade(tradeEvent.getTrade());
}

bool FootPrintEngine::processTrade(const Trade& trade) {
    if (tickSize <= 0.0) {
        return false;
    }
    if (trade.getQuantity() <= 0) {
        return false;
    }
    if (!symbolFilter.empty() && trade.getSymbol() != symbolFilter) {
        return false;
    }

    const Tick tick = toTick(trade.getPrice());
    auto& level = footPrintLevels.try_emplace(tick).first->second;
    return level.processTrade(trade);
}

const FootPrintLevel* FootPrintEngine::getLevel(double price) const {
    auto it = findTick(price);
    if (it == footPrintLevels.end()) {
        return nullptr;
    }
    return &it->second;
}

const FootPrintLevel* FootPrintEngine::getPrevLevel(double price) const {
    auto it = findTick(price);
    if (it == footPrintLevels.end() || it == footPrintLevels.begin()) {
        return nullptr;
    }
    return &std::prev(it)->second;
}

const FootPrintLevel* FootPrintEngine::getNextLevel(double price) const {
    auto it = findTick(price);
    if (it == footPrintLevels.end()) {
        return nullptr;
    }
    auto next = std::next(it);
    if (next == footPrintLevels.end()) {
        return nullptr;
    }
    return &next->second;
}

double FootPrintEngine::getTickSize() const {
    return tickSize;
}

const std::string& FootPrintEngine::getSymbolFilter() const {
    return symbolFilter;
}

size_t FootPrintEngine::getLevelCount() const {
    return footPrintLevels.size();
}

FootPrintSnapshot FootPrintEngine::snapshot() const {
    FootPrintSnapshot snap;
    if (footPrintLevels.empty()) {
        return snap;
    }

    snap.levelCount = footPrintLevels.size();
    snap.lowPrice = toPrice(footPrintLevels.begin()->first);
    snap.highPrice = toPrice(footPrintLevels.rbegin()->first);

    for (const auto& entry : footPrintLevels) {
        const FootPrintLevel& level = entry.second;
        snap.totalBuyVolume += level.getBuyVolume();
        snap.totalSellVolume += level.getSellVolume();
        snap.totalDelta += level.getDelta();
    }
    snap.totalVolume = snap.totalBuyVolume + snap.totalSellVolume;
    return snap;
}

void FootPrintEngine::reset() {
    footPrintLevels.clear();
}
