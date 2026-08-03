#include "TradeEvent.h"

TradeEvent::TradeEvent(EventId eventId,
                       SequenceNumber sequenceNumber,
                       Exchange exchangeName,
                       const Trade& trade)
    : id(eventId),
      sequenceNumber(sequenceNumber),
      exchangeName(exchangeName),
      trade(trade),
      symbolName(trade.getSymbol()) {}

SequenceNumber TradeEvent::sequence() const {
    return sequenceNumber;
}

Exchange TradeEvent::exchange() const {
    return exchangeName;
}

EventId TradeEvent::eventId() const {
    return id;
}

EventType TradeEvent::type() const {
    return EventType::Trade;
}

Timestamp TradeEvent::timestamp() const {
    return trade.getTradeTime();
}

const Symbol& TradeEvent::symbol() const {
    return symbolName;
}

const Trade& TradeEvent::getTrade() const {
    return trade;
}