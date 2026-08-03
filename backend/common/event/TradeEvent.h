#pragma once

#include "MarketEvent.h"
#include "Trade.h"

class TradeEvent : public MarketEvent {
private:
    EventId id;
    SequenceNumber sequenceNumber;
    Exchange exchangeName;
    Trade trade;
    Symbol symbolName;

public:
    TradeEvent(EventId eventId,
               SequenceNumber sequenceNumber,
               Exchange exchangeName,
               const Trade& trade);

    SequenceNumber sequence() const override;
    Exchange exchange() const override;
    EventId eventId() const override;
    EventType type() const override;
    Timestamp timestamp() const override;
    const Symbol& symbol() const override;
    const Trade& getTrade() const;
};