#pragma once

#include "MarketDataAdapter.h"

// Placeholder adapters for Indian equity venues (Step 3b / 3c).

class NseAdapter : public MarketDataAdapter {
public:
    using MarketDataAdapter::MarketDataAdapter;
    void start() override;
    void stop() override;
};

class BseAdapter : public MarketDataAdapter {
public:
    using MarketDataAdapter::MarketDataAdapter;
    void start() override;
    void stop() override;
};
