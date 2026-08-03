#pragma once

#include "MarketEvent.h"

class AnalysisEngine {
public:
    virtual ~AnalysisEngine() = default;
    virtual void onEvent(const MarketEvent& event) = 0;
    virtual void reset() = 0;
};