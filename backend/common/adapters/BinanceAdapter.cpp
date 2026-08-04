#include "BinanceAdapter.h"

#include "BinanceHttpClient.h"
#include "BinanceJsonParser.h"
#include "BinanceWebSocketClient.h"
#include "TradeEvent.h"

#include <iostream>

BinanceAdapter::BinanceAdapter(EventDispatcher& dispatcher, std::string symbol, int tradeLimit)
    : MarketDataAdapter(dispatcher, Exchange::Binance),
      symbol(std::move(symbol)),
      tradeLimit(tradeLimit) {}

void BinanceAdapter::emitParsedTrade(const ParsedBinanceTrade& parsed) {
    if (!parsed.valid) {
        return;
    }

    // buyOrderId / sellOrderId are synthetic — venue only gives one trade id.
    const int buyOrderId = static_cast<int>(parsed.tradeId);
    const int sellOrderId = static_cast<int>(parsed.tradeId + 1);

    Trade trade(buyOrderId,
                sellOrderId,
                parsed.symbol,
                parsed.price,
                parsed.quantity,
                parsed.aggressorSide,
                parsed.tradeTime);

    TradeEvent event(nextEventId++, nextSequence++, Exchange::Binance, trade);
    emit(event);
}

void BinanceAdapter::ingestWebSocketMessage(const std::string& json) {
    const ParsedBinanceTrade parsed = parseBinanceWebSocketTrade(json);
    emitParsedTrade(parsed);
}

void BinanceAdapter::ingestRestTradesJson(const std::string& jsonArray) {
    const std::vector<ParsedBinanceTrade> trades = parseBinanceRestTradesArray(jsonArray, symbol);
    for (const ParsedBinanceTrade& parsed : trades) {
        emitParsedTrade(parsed);
    }
}

void BinanceAdapter::start() {
    if (running) {
        return;
    }
    running = true;

    // Step A: fetch raw JSON from venue (REST snapshot).
    const std::string jsonBody = fetchBinanceRecentTrades(symbol, tradeLimit);
    if (jsonBody.empty()) {
        std::cerr << "BinanceAdapter: failed to fetch trades for " << symbol
                  << " (check internet, curl.exe in PATH, and symbol spelling)\n";
        return;
    }

    // Step B: JSON array → ParsedBinanceTrade list.
    const std::vector<ParsedBinanceTrade> trades = parseBinanceRestTradesArray(jsonBody, symbol);

    // Step C: same conversion as SimulatedAdapter — Trade → TradeEvent → emit().
    for (const ParsedBinanceTrade& parsed : trades) {
        emitParsedTrade(parsed);
    }
}

int BinanceAdapter::startStreaming(int durationSeconds, int maxTrades) {
    if (running) {
        return 0;
    }
    running = true;

    std::cout << "BinanceAdapter: streaming " << symbol
              << " for up to " << durationSeconds << "s / " << maxTrades << " trades...\n";

    // Live path: each WebSocket JSON line → same emitParsedTrade() as REST.
    const int delivered = BinanceWebSocketClient::streamTrades(
        symbol,
        durationSeconds,
        maxTrades,
        [this](const std::string& jsonLine) {
            ingestWebSocketMessage(jsonLine);
        });

    std::cout << "BinanceAdapter: streamed " << delivered << " trade message(s)\n";
    return delivered;
}

void BinanceAdapter::stop() {
    running = false;
}

bool BinanceAdapter::isRunning() const {
    return running;
}
