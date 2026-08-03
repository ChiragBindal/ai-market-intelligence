#pragma once

#include <vector>

class MarketEvent;
class AnalysisEngine;

class EventDispatcher {
public:
    void subscribe(AnalysisEngine* engine);
    void publish(const MarketEvent& event);

private:
    std::vector<AnalysisEngine*> subscribers;
};