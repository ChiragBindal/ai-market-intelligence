#include "BinanceJsonParser.h"

#include <cctype>
#include <stdexcept>

namespace {

std::string::size_type findKey(const std::string& json, const std::string& key) {
    const std::string quoted = "\"" + key + "\"";
    return json.find(quoted);
}

std::string::size_type valueStartAfterColon(const std::string& json, std::string::size_type keyPos) {
    const auto colon = json.find(':', keyPos);
    if (colon == std::string::npos) {
        return std::string::npos;
    }
    auto pos = colon + 1;
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) {
        ++pos;
    }
    return pos;
}

std::chrono::system_clock::time_point msToTimePoint(int64_t epochMs) {
    using Ms = std::chrono::milliseconds;
    return std::chrono::system_clock::time_point(Ms(epochMs));
}

double parseDouble(const std::string& text) {
    try {
        return std::stod(text);
    } catch (const std::exception&) {
        return 0.0;
    }
}

int64_t parseInt64(const std::string& text) {
    try {
        return std::stoll(text);
    } catch (const std::exception&) {
        return 0;
    }
}

// Portable integer parse from JSON number at position (no std::from_chars - old MinGW friendly).
bool parseInt64At(const std::string& json, std::string::size_type valuePos, int64_t& out) {
    if (valuePos == std::string::npos || valuePos >= json.size()) {
        return false;
    }
    std::string::size_type end = valuePos;
    while (end < json.size() && (std::isdigit(static_cast<unsigned char>(json[end])) || json[end] == '-')) {
        ++end;
    }
    if (end == valuePos) {
        return false;
    }
    out = parseInt64(json.substr(valuePos, end - valuePos));
    return true;
}

}  // namespace

std::optional<std::string> extractJsonString(const std::string& json, const std::string& key) {
    const auto keyPos = findKey(json, key);
    if (keyPos == std::string::npos) {
        return std::nullopt;
    }
    const auto valuePos = valueStartAfterColon(json, keyPos);
    if (valuePos == std::string::npos || valuePos >= json.size() || json[valuePos] != '"') {
        return std::nullopt;
    }
    const auto endQuote = json.find('"', valuePos + 1);
    if (endQuote == std::string::npos) {
        return std::nullopt;
    }
    return json.substr(valuePos + 1, endQuote - valuePos - 1);
}

std::optional<bool> extractJsonBool(const std::string& json, const std::string& key) {
    const auto keyPos = findKey(json, key);
    if (keyPos == std::string::npos) {
        return std::nullopt;
    }
    const auto valuePos = valueStartAfterColon(json, keyPos);
    if (valuePos == std::string::npos) {
        return std::nullopt;
    }
    if (json.compare(valuePos, 4, "true") == 0) {
        return true;
    }
    if (json.compare(valuePos, 5, "false") == 0) {
        return false;
    }
    return std::nullopt;
}

std::optional<int64_t> extractJsonInt64(const std::string& json, const std::string& key) {
    const auto keyPos = findKey(json, key);
    if (keyPos == std::string::npos) {
        return std::nullopt;
    }
    const auto valuePos = valueStartAfterColon(json, keyPos);
    int64_t value = 0;
    if (!parseInt64At(json, valuePos, value)) {
        return std::nullopt;
    }
    return value;
}

std::optional<double> extractJsonNumber(const std::string& json, const std::string& key) {
    const auto str = extractJsonString(json, key);
    if (str.has_value()) {
        return parseDouble(*str);
    }
    const auto keyPos = findKey(json, key);
    if (keyPos == std::string::npos) {
        return std::nullopt;
    }
    const auto valuePos = valueStartAfterColon(json, keyPos);
    int64_t intValue = 0;
    if (!parseInt64At(json, valuePos, intValue)) {
        return std::nullopt;
    }
    return static_cast<double>(intValue);
}

Side aggressorFromBuyerIsMaker(bool buyerIsMaker) {
    return buyerIsMaker ? Side::SELL : Side::BUY;
}

ParsedBinanceTrade parseBinanceWebSocketTrade(const std::string& json) {
    ParsedBinanceTrade parsed;
    const auto eventType = extractJsonString(json, "e");
    if (!eventType.has_value() || *eventType != "trade") {
        return parsed;
    }

    const auto symbol = extractJsonString(json, "s");
    const auto tradeId = extractJsonInt64(json, "t");
    const auto price = extractJsonNumber(json, "p");
    const auto quantity = extractJsonNumber(json, "q");
    const auto tradeTimeMs = extractJsonInt64(json, "T");
    const auto buyerIsMaker = extractJsonBool(json, "m");

    if (!symbol.has_value() || !tradeId.has_value() || !price.has_value() || !quantity.has_value()
        || !tradeTimeMs.has_value() || !buyerIsMaker.has_value()) {
        return parsed;
    }

    parsed.tradeId = *tradeId;
    parsed.symbol = *symbol;
    parsed.price = *price;
    parsed.quantity = *quantity;
    parsed.tradeTime = msToTimePoint(*tradeTimeMs);
    parsed.aggressorSide = aggressorFromBuyerIsMaker(*buyerIsMaker);
    parsed.valid = parsed.quantity > 0.0;
    return parsed;
}

ParsedBinanceTrade parseBinanceRestTrade(const std::string& jsonObject, const std::string& symbol) {
    ParsedBinanceTrade parsed;
    const auto tradeId = extractJsonInt64(jsonObject, "id");
    const auto price = extractJsonNumber(jsonObject, "price");
    const auto quantity = extractJsonNumber(jsonObject, "qty");
    const auto tradeTimeMs = extractJsonInt64(jsonObject, "time");
    const auto buyerIsMaker = extractJsonBool(jsonObject, "isBuyerMaker");

    if (!tradeId.has_value() || !price.has_value() || !quantity.has_value()
        || !tradeTimeMs.has_value() || !buyerIsMaker.has_value()) {
        return parsed;
    }

    parsed.tradeId = *tradeId;
    parsed.symbol = symbol;
    parsed.price = *price;
    parsed.quantity = *quantity;
    parsed.tradeTime = msToTimePoint(*tradeTimeMs);
    parsed.aggressorSide = aggressorFromBuyerIsMaker(*buyerIsMaker);
    parsed.valid = parsed.quantity > 0.0;
    return parsed;
}

std::vector<ParsedBinanceTrade> parseBinanceRestTradesArray(const std::string& jsonArray,
                                                            const std::string& symbol) {
    std::vector<ParsedBinanceTrade> trades;
    std::string::size_type searchFrom = 0;

    while (true) {
        const auto objectStart = jsonArray.find('{', searchFrom);
        if (objectStart == std::string::npos) {
            break;
        }
        const auto objectEnd = jsonArray.find('}', objectStart);
        if (objectEnd == std::string::npos) {
            break;
        }

        const std::string object = jsonArray.substr(objectStart, objectEnd - objectStart + 1);
        ParsedBinanceTrade parsed = parseBinanceRestTrade(object, symbol);
        if (parsed.valid) {
            trades.push_back(parsed);
        }
        searchFrom = objectEnd + 1;
    }

    return trades;
}
