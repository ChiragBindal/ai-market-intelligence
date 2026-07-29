#include "InitialBalanceEngine.h"
#include <algorithm>
#include <limits>

InitialBalanceEngine::InitialBalanceEngine(Timestamp sessionStart, Duration initialBalanceWindow)
    : sessionStartTime(sessionStart), 
    initialBalanceWindow(initialBalanceWindow),
    ibHigh(std::numeric_limits<Price>::lowest()),
    ibLow(std::numeric_limits<Price>::max()){
}

void InitialBalanceEngine::processTrade(const Trade& trade){
    if(isFinalized){
        return;
    }
    if(hasInitialBalanceWindowEnded(trade)){
        finalizeInitialBalance();
        return;
    }
    updateInitialBalance(trade);
}

Price InitialBalanceEngine::getInitialBalanceHigh() const{
    return ibHigh;
}

Price InitialBalanceEngine::getInitialBalanceLow() const{
    return ibLow;
}

bool InitialBalanceEngine::isInitialBalanceFinalized() const{
    return isFinalized;
}
void InitialBalanceEngine::updateInitialBalance(const Trade& trade){
    ibHigh = std::max(ibHigh, trade.getPrice());
    ibLow = std::min(ibLow, trade.getPrice());
}
bool InitialBalanceEngine::hasInitialBalanceWindowEnded(const Trade& trade) const{
    // Trades at exactly sessionStart + window (e.g. 10:15 for a 09:15 session with 60 min)
    // are excluded from IB because the comparison is >=.
    return trade.getTradeTime() >= sessionStartTime + initialBalanceWindow;
}
void InitialBalanceEngine::finalizeInitialBalance(){
    isFinalized = true;
}