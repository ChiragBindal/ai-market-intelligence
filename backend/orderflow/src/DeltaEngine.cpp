#include "DeltaEngine.h"
#include "TradeEvent.h"

DeltaEngine::DeltaEngine(size_t rollingWindowSize, std::string symbolFilter)
    : buyVolume(0),
      sellVolume(0),
      delta(0),
      rollingWindowSize(rollingWindowSize),
      symbolFilter(std::move(symbolFilter)) {}

void DeltaEngine::addObservation(double buyQty, double sellQty) {
    buyVolume += buyQty;
    sellVolume += sellQty;
    delta += buyQty - sellQty;
    observations.push_back(Observation{buyQty, sellQty});
    trimWindow();
}

void DeltaEngine::trimWindow() {
    if (rollingWindowSize == 0) {
        return;
    }
    while (observations.size() > rollingWindowSize) {
        const Observation& oldest = observations.front();
        buyVolume -= oldest.buyQty;
        sellVolume -= oldest.sellQty;
        delta -= oldest.buyQty - oldest.sellQty;
        observations.pop_front();
    }
}

bool DeltaEngine::processTrade(const Trade& trade) {
    if (trade.getQuantity() <= 0) {
        return false;
    }
    if (!symbolFilter.empty() && trade.getSymbol() != symbolFilter) {
        return false;
    }

    if (trade.getAggressorSide() == Side::BUY) {
        addObservation(trade.getQuantity(), 0.0);
    } else {
        addObservation(0.0, trade.getQuantity());
    }
    return true;
}

void DeltaEngine::onEvent(const MarketEvent& event) {
    if (event.type() != EventType::Trade) {
        return;
    }
    const auto& tradeEvent = static_cast<const TradeEvent&>(event);
    processTrade(tradeEvent.getTrade());
}

double DeltaEngine::getBuyVolume() const {
    return buyVolume;
}

double DeltaEngine::getSellVolume() const {
    return sellVolume;
}

double DeltaEngine::getDelta() const {
    return delta;
}

double DeltaEngine::getTotalVolume() const {
    return buyVolume + sellVolume;
}

double DeltaEngine::getDeltaPct() const {
    const double total = getTotalVolume();
    if (total <= 0.0) {
        return 0.0;
    }
    return delta / total;
}

DeltaSnapshot DeltaEngine::snapshot() const {
    return DeltaSnapshot{
        buyVolume,
        sellVolume,
        delta,
        getTotalVolume(),
        getDeltaPct()
    };
}

size_t DeltaEngine::getRollingWindowSize() const {
    return rollingWindowSize;
}

size_t DeltaEngine::getObservationCount() const {
    return observations.size();
}

const std::string& DeltaEngine::getSymbolFilter() const {
    return symbolFilter;
}

void DeltaEngine::reset() {
    buyVolume = 0;
    sellVolume = 0;
    delta = 0;
    observations.clear();
}
