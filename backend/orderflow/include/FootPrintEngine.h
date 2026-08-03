#pragma once

#include "AnalysisEngine.h"
#include "FootPrintLevel.h"
#include "Trade.h"

#include <cstdint>
#include <map>
#include <string>

// Fusion-ready summary of the current footprint.
// Price ordering: prev = lower price, next = higher price.
struct FootPrintSnapshot {
    double lowPrice = 0.0;
    double highPrice = 0.0;
    double totalBuyVolume = 0.0;
    double totalSellVolume = 0.0;
    double totalDelta = 0.0;
    double totalVolume = 0.0;
    size_t levelCount = 0;
};

class FootPrintEngine : public AnalysisEngine {
private:
    using Tick = int64_t;

    std::map<Tick, FootPrintLevel> footPrintLevels;
    double tickSize;
    std::string symbolFilter;   // empty = accept all symbols

    Tick toTick(double price) const;
    double toPrice(Tick tick) const;
    std::map<Tick, FootPrintLevel>::const_iterator findTick(double price) const;

public:
    // tickSize groups nearby prices onto the same level (e.g. 0.05 for NSE).
    explicit FootPrintEngine(double tickSize = 0.05, std::string symbolFilter = "");

    void onEvent(const MarketEvent& event) override;

    // Returns false if skipped (bad qty / wrong symbol / invalid tickSize).
    bool processTrade(const Trade& trade);

    // Lookup by price (rounded to tick). nullptr if level does not exist.
    const FootPrintLevel* getLevel(double price) const;

    // Adjacent levels in price order: prev = lower price, next = higher price.
    const FootPrintLevel* getPrevLevel(double price) const;
    const FootPrintLevel* getNextLevel(double price) const;

    double getTickSize() const;
    const std::string& getSymbolFilter() const;
    size_t getLevelCount() const;
    FootPrintSnapshot snapshot() const;

    void reset() override;
};
