#pragma once

enum class EventType {
    Trade,
    OrderAdded,
    OrderModified,
    OrderCancelled,
    Quote,
    Candle,
    SessionStart,
    SessionEnd
};
