#pragma once

#include "FootPrintEngine.h"
#include "ImbalanceEngine.h"

class AbsorptionEngine {
private:
    const FootPrintEngine& m_footprint;
    const ImbalanceEngine& m_imbalance;
    double volumeThreshold;

public:
    AbsorptionEngine(const FootPrintEngine& footprint, const ImbalanceEngine& imbalance, double volumeThreshold);
    bool isSellAbsorption(double price) const;
    bool isBuyAbsorption(double price) const;
};
