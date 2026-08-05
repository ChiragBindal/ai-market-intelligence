#pragma once

#include "EventDispatcher.h"
#include "Exchange.h"
#include "MarketEvent.h"

// =============================================================================
// MarketDataAdapter — base class for all exchange feeds (ATHENA whiteboard: "Adapter")
// =============================================================================
//
// MARKET PRINCIPLE — Why adapters exist:
//   NSE, BSE, and Binance all send market data in *different formats* (JSON fields,
//   timestamps, symbol naming). Our analytics engines (Delta, Footprint, etc.)
//   should NOT care which exchange the data came from. They only need a uniform
//   MarketEvent (e.g. TradeEvent).
//
//   Adapter = translator. Raw venue message → normalized MarketEvent → EventDispatcher.
//
// DESIGN PATTERN — Adapter + inheritance:
//   - This class holds shared wiring (dispatcher, exchange id, event id counters).
//   - Concrete classes (SimulatedAdapter, NseAdapter, ...) implement start()/stop()
//     and call emit() when they have a normalized event.
//
// RELATION TO EVENT BUS (already done — we do NOT change EventDispatcher):
//   emit() → dispatcher.publish() → every subscribed AnalysisEngine gets onEvent().
// =============================================================================
class MarketDataAdapter {
protected:
    // Reference to the event bus. All adapters publish into the same dispatcher.
    EventDispatcher& dispatcher;

    // Which venue this feed represents (NSE, BSE, Binance, ...).
    Exchange exchange;

    // Monotonic ids so every event is unique and ordered within this adapter instance.
    EventId nextEventId{1};
    SequenceNumber nextSequence{1};

    // Publishes a normalized event to all subscribed analysis engines.
    // Concrete adapters call this instead of touching EventDispatcher directly.
    void emit(const MarketEvent& event);

public:
    MarketDataAdapter(EventDispatcher& dispatcher, Exchange exchange);
    virtual ~MarketDataAdapter() = default;

    Exchange getExchange() const;

    // start() — connect to feed / replay ticks / begin parsing.
    // stop()  — disconnect cleanly.
    virtual void start() = 0;
    virtual void stop() = 0;
};
