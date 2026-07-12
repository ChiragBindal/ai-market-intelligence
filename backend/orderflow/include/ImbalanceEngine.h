class ImbalanceEngine {
    private:
    const FootPrintEngine& footPrintEngine;
    double imbalanceRatio;

    public:
    ImbalanceEngine(const FootPrintEngine& footPrintEngine, double imbalanceRatio=3.0);
    bool isBuyImbalance(double price) const;
    bool isSellImbalance(double price) const;
}