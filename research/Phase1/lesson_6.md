Motive why institutions hide orders , how exchange support it and how traders detect it
## Ice Berg orders
- Suppose mutual fund came and it want to buy 1,00,000 shares , if it buy order at one time , then liquidity will be removed and price will increase , -> Market Impact.
- Second retail traders would identify that someone big is investing so they try to move price (ie they would also buy)
- Solution : 1000 share at a time -> executed -> refresh -> 1000 share .... cycle continue until all 1,00,000 share is executed. 
 So trader just see 1000 share (like an iceberg)
- Engineering : class Iceberg : public Order{
    private : 
    int initial_qty;
    int total_qty;
    int hidden_qty;

    public:
    Iceberg(int initial_qty , int total_qty);
    void displayInitialQty(); -> initial_qty;
    void refreshInitialAfterExecuted() -> initital_qty = x , hidden -= x;
}
- Product : We can see 1000 share being executed many times , 
    possibility - iceberg , conincidence , execution algorithm submitting 500 again and again