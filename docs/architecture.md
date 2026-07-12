Order Book 
- Contain Sell order
- Contain Buy Order

Sell Order Entries : Increasing order , since least sell price order must be executed first
Data Structure -> priority_queue_min<Price , DLL<Order>>
                  DLL : FIFO
                Q. How would we delete order , if someone cancelled it ? map<Order,ListNode*> addressOfOrder
                We will add new order to end of DLL.

Buy Order Entries : Decreasing order , largest value order must be executed first
Data Structure -> priority_queue_max<Price , DLL<Order>>
                  DLL : FIFO

Now if someone ask how many order of certain price
PriceLevel : len : int , DLL<Order>

Buy OrderBook -> map_max<Price , PriceLevel>
Sell OrderBook -> map_min<Price , PriceLevel>

Order Status : enum -> NEW , VALIDATED , ACCEPTED , PARTIALLY_FILLED , FILLED , REJECTED
One think to notice since we are making exchange so order status - NEW , VALIDATED : not needed

ORDER_TYPE : enum -> LIMIT , MARKET

ORDER : order_id , price , buy/sell order flag , symbol , original_quantity , Order_type , Order_status , excuted_order , remaining_order , timestamp

TRADE : tradeId , price , buyOrderId , sellOrderId , qty_traded , timestamp

Matching Engine : 
- order came , check if order_type == LIMIT , --> enter into orderbook acc to flag of buy/sell
- else immediately execute order for <= price

Flow : orderBook.add(order) -> then add to buy / sell orderbook 
best ask , best buy from orderbook 

buy order will search for sell order which are less than == to ask price -> Trade 

Analytics PipeLine : 
Trade
   │
   ▼
Delta
   │
   ▼
Footprint
   │
   ▼
Imbalance
   │
   ▼
Absorption
   │
   ▼
Exhaustion
   │
   ▼
Liquidity
   │
   ▼
Volume Profile
   │
   ▼
Signal Fusion
   │
   ▼
AI Reasoning