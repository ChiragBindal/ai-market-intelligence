#include "EventDispatcher.h"
#include "AnalysisEngine.h"
#include "MarketEvent.h"

void EventDispatcher::subscribe(AnalysisEngine* engine) {
    subscribers.push_back(engine);
}

void EventDispatcher::publish(const MarketEvent& event) {
    for (AnalysisEngine* engine : subscribers) {
        engine->onEvent(event);
    }
}