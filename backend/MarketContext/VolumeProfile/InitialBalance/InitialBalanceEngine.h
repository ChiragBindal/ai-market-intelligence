#pragma once

#include <chrono>
#include "Trade.h"

using Price = double;
using Timestamp = std::chrono::system_clock::time_point;
using Duration = std::chrono::minutes;

class InitialBalanceEngine {
private:
    Price ibHigh{};
    Price ibLow{};
    Timestamp sessionStartTime;
    Duration initialBalanceWindow;
    bool isFinalized{false};
    bool tradedAboveIB{false};
    bool tradedBelowIB{false};

    void updateInitialBalance(const Trade& trade);
    bool hasInitialBalanceWindowEnded(const Trade& trade) const;
    void finalizeInitialBalance();
    void monitorAcceptance(const Trade& trade);

public:
    InitialBalanceEngine(Timestamp sessionStart, Duration initialBalanceWindow);
    void processTrade(const Trade& trade);
    Price getInitialBalanceHigh() const;
    Price getInitialBalanceLow() const;
    bool isInitialBalanceFinalized() const;
    bool hasTradedAboveIB() const;
    bool hasTradedBelowIB() const;
    void reset();
};
