## Continous Double Action
- Tells how the matching engine works
- if (ORDER_TYPE == MARKET) : It need to implement immediately , execute the avaible quantity at price <= ORDER_GIVEN_PRICE and remaining quantity = CANCELLED
- if (ORDER_TYPE == LIMIT) : It execute available quantity from orderbook at price <= ORDER_GIVE_PRICE , and if(remainingQuantity > 0) = Enter into orderBook
- Algorithm : 
Incoming Order
↓
Look at opposite OrderBook(if Order == Buy , Then look for Sell OrderBook)
↓
Can match?
↓
YES
↓
Execute Trade
↓
Update Orders
↓
Remove completed orders
↓
Remove empty price levels
↓
Repeat until
• no quantity left OR
• no matching prices
↓
If LIMIT and quantity remains
↓
Add remaining quantity to OrderBook
↓
If MARKET and quantity remains
↓
Cancel remaining quantity

## Priority Time Based 
Suppose two order came 
1. Sell 100 qty @ 100 ruppess , time 2:15:01
2. Sell 200 qty @ 100 ruppess , time 2:15:05
which one will be executed first ? - 100 qty @ 100 ruppess
Since exchange give priority on the basis of time , FIFO (First In First Out). It gives Fairness, Incentive to add liquidity, Deterministic

## Queue Position
- How many orders are ahead of our order at same price level
- Goal for product to show : 
    Order Status
        Position in Queue: 642 shares
        Estimated Fill Probability (next 5 min): 18%
        Estimated Wait Time: 4.2 minutes

## Market Makers
- Goal : Stay liquid, manage inventory, and earn the spread while controlling risk.
- Most people think makrket is just about buyers and sellers (Wrong) , there is third player - market makers
- Suppose market open with 
    Sell OrderBook (empty) , BuyOrderBook (empty)
    Here enter market maker always ready to buy and sell
    1000@99.90 buy , 1000@100.10 sell (now market hase liquidity)
- Spread = Best Ask - Best bid = 0.20 (this there earning) - capturing the spread
- But suddenly bad news/something happen - Inventory Risk
- Suppose 1000@100 rupess BUY , 1000@100.20 rupess SELL and now the most seller came and sell to you
    1. There would be lot of inventory for use 5000 share now suddenly market falls and selling price become 95 so there would be loss of 5*5000
    2. Should lower my bid(buy) 99.70 so sellers would be less interested
    3. Should lower my ask(sell) 100.05 
- Optimization Problem : "What bid and ask prices maximize my expected profit while keeping inventory risk under control?"
- if (inventory > MAX_LONG) {
    lowerBid();  prev : BUY 100@100 rupess , after : BUY 100@99.70
    lowerAsk();  prev : SELL 100@100.20 , after SELL 100@100.05
    reduceBidQuantity(); prev : BUY 100@100 rupess , after : BUY 20@99.70
    widenSpread();
}
