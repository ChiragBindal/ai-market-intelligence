#include "SimulatedAdapter.h"
#include "TradeEvent.h"

#include <chrono>

SimulatedAdapter::SimulatedAdapter(EventDispatcher& dispatcher,
                                   Exchange exchange,
                                   std::vector<SimulatedTradeTick> ticks)
    : MarketDataAdapter(dispatcher, exchange), ticks(std::move(ticks)) {}

void SimulatedAdapter::start() {
    // Idempotent: calling start() twice should not double-publish ticks.
    if (running) {
        return;
    }
    running = true;

    const auto now = std::chrono::system_clock::now();
    int tradeCounter = 1;

    for (const SimulatedTradeTick& tick : ticks) {
        // Build domain Trade (exchange layer model) from our simple tick struct.
        // buyOrderId / sellOrderId are synthetic here — real adapters map from venue ids.
        Trade trade(tradeCounter,
                    tradeCounter + 1,
                    tick.symbol,
                    tick.price,
                    tick.quantity,
                    tick.aggressorSide,
                    now);

        // Wrap Trade in TradeEvent — the normalized event type engines understand.
        TradeEvent event(nextEventId++, nextSequence++, exchange, trade);

        // Publish to event bus → DeltaEngine / FootPrintEngine / etc.
        emit(event);

        // Synthetic ids only; real feeds use venue trade ids.
        tradeCounter += 2;
    }
}

void SimulatedAdapter::stop() {
    running = false;
}

bool SimulatedAdapter::isRunning() const {
    return running;
}
