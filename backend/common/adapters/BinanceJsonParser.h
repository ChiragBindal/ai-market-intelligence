#pragma once

#include "Side.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// Parsed fields from a Binance trade message (WebSocket or REST).
// This is our intermediate format — same role as SimulatedTradeTick.
struct ParsedBinanceTrade {
    int64_t tradeId{0};
    std::string symbol;
    double price{0.0};
    double quantity{0.0};
    Side aggressorSide{Side::BUY};
    std::chrono::system_clock::time_point tradeTime;
    bool valid{false};
};

// Minimal JSON helpers — we only extract known Binance keys (no external JSON library).
std::optional<std::string> extractJsonString(const std::string& json, const std::string& key);
std::optional<bool> extractJsonBool(const std::string& json, const std::string& key);
std::optional<int64_t> extractJsonInt64(const std::string& json, const std::string& key);
std::optional<double> extractJsonNumber(const std::string& json, const std::string& key);

// Aggressor from Binance "is buyer the market maker?" flag.
// m / isBuyerMaker == true  → buyer was maker → seller hit bid → aggressive SELL
// m / isBuyerMaker == false → buyer was taker  → aggressive BUY
Side aggressorFromBuyerIsMaker(bool buyerIsMaker);

// WebSocket trade stream: {"e":"trade","s":"BTCUSDT","t":...,"p":"...","q":"...","T":...,"m":...}
ParsedBinanceTrade parseBinanceWebSocketTrade(const std::string& json);

// REST recent trades: one object {"id":...,"price":"...","qty":"...","time":...,"isBuyerMaker":...}
// symbol comes from adapter config (REST payload does not repeat symbol per trade).
ParsedBinanceTrade parseBinanceRestTrade(const std::string& jsonObject, const std::string& symbol);

// REST response is a JSON array — split and parse each trade object.
std::vector<ParsedBinanceTrade> parseBinanceRestTradesArray(const std::string& jsonArray,
                                                            const std::string& symbol);
