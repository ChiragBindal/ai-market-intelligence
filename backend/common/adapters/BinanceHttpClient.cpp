#include "BinanceHttpClient.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#ifdef _WIN32
// On Windows, "curl" in cmd may resolve to PowerShell alias — always use curl.exe.
const char* kCurlBinary = "curl.exe";
#else
const char* kCurlBinary = "curl";
#endif

// Fetches recent trades via public Binance REST API using curl + temp file.
std::string fetchBinanceRecentTrades(const std::string& symbol, int limit) {
    if (limit <= 0 || symbol.empty()) {
        return {};
    }

    const std::string url =
        "https://api.binance.com/api/v3/trades?symbol=" + symbol + "&limit=" + std::to_string(limit);
    const std::string tmpPath = "athena_binance_trades.json";
    const std::string cmd =
        std::string(kCurlBinary) + " -s \"" + url + "\" > \"" + tmpPath + "\"";

    if (std::system(cmd.c_str()) != 0) {
        return {};
    }

    std::ifstream in(tmpPath);
    if (!in) {
        return {};
    }

    std::ostringstream body;
    body << in.rdbuf();
    in.close();
    std::remove(tmpPath.c_str());
    return body.str();
}
