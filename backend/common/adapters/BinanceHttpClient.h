#pragma once

#include <string>

// Fetches Binance REST recent trades for a symbol.
// GET https://api.binance.com/api/v3/trades?symbol=BTCUSDT&limit=N
// Returns raw JSON array body, or empty string on failure.
std::string fetchBinanceRecentTrades(const std::string& symbol, int limit);
