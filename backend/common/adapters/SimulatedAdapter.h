#pragma once

#include "MarketDataAdapter.h"
#include "Side.h"

#include <string>
#include <vector>

// One synthetic trade tick — input format for SimulatedAdapter (not a MarketEvent yet).
struct SimulatedTradeTick {
    std::string symbol;
    double price;
    double quantity;
    Side aggressorSide;  // BUY = buyer was aggressive (lifted the ask); SELL = hit the bid
};

// =============================================================================
// SimulatedAdapter — fake exchange feed for demos and tests
// =============================================================================
//
// MARKET PRINCIPLE — Aggressor side:
//   When a trade prints, one side was "aggressive" (crossed the spread):
//     - Aggressor BUY  → buyer paid the ask → counts toward buy volume / positive delta
//     - Aggressor SELL → seller hit the bid → counts toward sell volume / negative delta
//
// WHY THIS CLASS EXISTS:
//   Before NSE/BSE/Binance live feeds are wired, we need repeatable data to test
//   EventDispatcher → DeltaEngine (and later Footprint, etc.). SimulatedAdapter
//   replays a fixed list of ticks as TradeEvents — same path as a real adapter.
//
// FLOW:
//   SimulatedTradeTick[] → Trade → TradeEvent → emit() → EventDispatcher → engines
// =============================================================================
class SimulatedAdapter : public MarketDataAdapter {
private:
    std::vector<SimulatedTradeTick> ticks;
    bool running{false};

public:
    SimulatedAdapter(EventDispatcher& dispatcher,
                     Exchange exchange,
                     std::vector<SimulatedTradeTick> ticks);

    void start() override;
    void stop() override;
    bool isRunning() const;
};
