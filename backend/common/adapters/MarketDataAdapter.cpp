#include "MarketDataAdapter.h"

MarketDataAdapter::MarketDataAdapter(EventDispatcher& dispatcher, Exchange exchange)
    : dispatcher(dispatcher), exchange(exchange) {}

// Single choke point: every adapter publishes through here.
// Keeps EventDispatcher usage consistent and makes it easy to add logging/metrics later.
void MarketDataAdapter::emit(const MarketEvent& event) {
    dispatcher.publish(event);
}

Exchange MarketDataAdapter::getExchange() const {
    return exchange;
}
