#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include "Exchange.h"
#include "EventType.h"

using Timestamp = std::chrono::system_clock::time_point;
using EventId = uint64_t;
using SequenceNumber = uint64_t;
using Symbol = std::string;

class MarketEvent {
public:
    virtual ~MarketEvent() = default;
    virtual SequenceNumber sequence() const = 0;
    virtual Exchange exchange() const = 0;
    virtual EventId eventId() const = 0;
    virtual EventType type() const = 0;
    virtual Timestamp timestamp() const = 0;
    virtual const Symbol& symbol() const = 0;
};