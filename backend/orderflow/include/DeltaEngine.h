#pragma once

#include "AnalysisEngine.h"
#include "Trade.h"

#include <deque>
#include <string>

struct DeltaSnapshot {
    double buyVolume = 0.0;
    double sellVolume = 0.0;
    double delta = 0.0;
    double totalVolume = 0.0;
    double deltaPct = 0.0;  // delta / totalVolume, 0 if no volume
};

class DeltaEngine : public AnalysisEngine {
private:
    struct Observation {
        double buyQty = 0.0;
        double sellQty = 0.0;
    };

    double buyVolume;
    double sellVolume;
    double delta;
    size_t rollingWindowSize;   // 0 = unlimited (session cumulative)
    std::string symbolFilter;   // empty = accept all symbols
    std::deque<Observation> observations;

    void addObservation(double buyQty, double sellQty);
    void trimWindow();

public:
    // rollingWindowSize = 0 keeps full-session cumulative delta
    explicit DeltaEngine(size_t rollingWindowSize = 0, std::string symbolFilter = "");

    void onEvent(const MarketEvent& event) override;
    bool processTrade(const Trade& trade);  // false if skipped (bad qty / wrong symbol)

    double getBuyVolume() const;
    double getSellVolume() const;
    double getDelta() const;
    double getTotalVolume() const;
    double getDeltaPct() const;
    DeltaSnapshot snapshot() const;

    size_t getRollingWindowSize() const;
    size_t getObservationCount() const;
    const std::string& getSymbolFilter() const;

    void reset() override;
};
