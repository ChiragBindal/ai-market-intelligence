#pragma once

#include "BinanceJsonParser.h"
#include "MarketDataAdapter.h"

#include <string>

// =============================================================================
// BinanceAdapter — real Binance trades → TradeEvent → EventDispatcher
// =============================================================================
//
// MARKET PRINCIPLE:
//   On Binance, each public trade print is already matched by the exchange.
//   We do NOT run our local OrderBook here — we consume venue trade messages.
//
// DATA PATH (same after parse for REST and WebSocket):
//   Raw JSON → ParsedBinanceTrade → Trade → TradeEvent → emit()
//
// LIVE FEED:
//   REST snapshot: GET /api/v3/trades?symbol=BTCUSDT&limit=N   → start()
//   WebSocket live: wss://stream.binance.com:9443/ws/<sym>@trade → startStreaming()
//
// AGGRESSOR (Binance field m / isBuyerMaker):
//   true  → buyer was maker → seller was taker → Side::SELL
//   false → buyer was taker  → Side::BUY
// =============================================================================
class BinanceAdapter : public MarketDataAdapter {
private:
    std::string symbol;
    int tradeLimit;
    bool running{false};

    // Convert parsed venue trade → domain Trade → publish TradeEvent.
    void emitParsedTrade(const ParsedBinanceTrade& parsed);

public:
    BinanceAdapter(EventDispatcher& dispatcher,
                   std::string symbol,
                   int tradeLimit = 100);

    // REST snapshot of recent trades (newspaper).
    void start() override;
    void stop() override;
    bool isRunning() const;

    // Live WebSocket tape for durationSeconds (or until maxTrades).
    // Returns how many WebSocket trade messages were ingested.
    int startStreaming(int durationSeconds = 15, int maxTrades = 100);

    // For tests: feed one WebSocket JSON line without HTTP/network.
    void ingestWebSocketMessage(const std::string& json);

    // For tests: feed REST JSON array without HTTP.
    void ingestRestTradesJson(const std::string& jsonArray);
};
