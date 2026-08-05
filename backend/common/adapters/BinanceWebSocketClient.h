#pragma once

#include <functional>
#include <string>

// Streams Binance trade WebSocket messages into a callback.
// Windows: launches binance_ws_stream.ps1 (PowerShell ClientWebSocket) which
// appends one JSON trade per line to a temp file; this class polls that file.
//
// Why not a C++ WebSocket library?
//   MinGW 9.2 setup in this project has no Boost/Beast/OpenSSL stack wired yet.
//   PowerShell already ships with System.Net.WebSockets on Windows.
//   After each JSON line arrives, the path is identical to ingestWebSocketMessage().
class BinanceWebSocketClient {
public:
    using MessageHandler = std::function<void(const std::string& jsonLine)>;

    // symbol e.g. "BTCUSDT", durationSeconds how long to listen, maxTrades early stop.
    // Returns number of JSON lines successfully delivered to handler.
    static int streamTrades(const std::string& symbol,
                            int durationSeconds,
                            int maxTrades,
                            const MessageHandler& onMessage,
                            const std::string& scriptPath = "binance_ws_stream.ps1");
};
