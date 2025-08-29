//
//  mathcingengine.h
//  OrderBook
//
class Exchange{
private:
    OrderBook& orderBook;
    OrderManager& orderManager;
    
public:
    explicit Exchange(OrderBook& orderBook, OrderManager& orderManager)
    :orderManager{orderManager},
    orderBook{orderBook}
    {}
    
    void dummyAddOrder(std::unique_ptr<Order> order) {
//        Price price = order->GetPrice();
//        Quantity qty = order->GetQuantity();
//        Side side = order->GetSide();
//        OrderID id = order->GetOrderID();
//        
//        if (side == Side::Buy) {
//            auto it = orderBook.bids.find(price);
//            if (it == orderBook.bids.end()) {
//                orderBook.bids.emplace(price, PriceLevel(price));
//            }
//            orderBook.bids.at(price).AddOrder(id, qty);
//        } else {
//            auto it = orderBook.asks.find(price);
//            if (it == orderBook.asks.end()) {
//                orderBook.asks.emplace(price, PriceLevel(price));
//            }
//            orderBook.asks.at(price).AddOrder(id, qty);
//        }
//        
//        
//        
//        
//        orderManager.AddOrder(std::move(order));
    }
    
    void AddOrder(std::unique_ptr<Order> order) {
        Order* rawOrder = order.get();  // keep pointer before moving
        orderManager.AddOrder(std::move(order));

        Price price = rawOrder->GetPrice();
        Quantity quantity = rawOrder->GetQuantity();
        Side side = rawOrder->GetSide();
        OrderType ordertype = rawOrder->GetOrderType();
        
        switch (ordertype) {
            case OrderType::Limit:
                LimitMatch(side, quantity, price);
                break;
            case OrderType::Market:
                MarketMatch(side, quantity);
                break;
            case OrderType::FillOrKill:
                // call FOK logic
                break;
            case OrderType::ImmediateOrCancel:
                // call IOC logic
                break;
            case OrderType::INVALID:
                throw std::runtime_error("INVALID ORDER TYPE");
                break;
        }
    }
    
    void MarketMatch(Side side, Quantity quantity){
        if (side == Side::Buy){
            isEmpty()
        } else {
            
        }
        while (quantity > 0 and !orderBook.AsksEmpty()){
            PriceLevel& bestAskLevel = orderBook.BestAskLevel();
            OrderIDs& orderIDs = bestAskLevel.GetOrderIDs();
            while (quantity > 0 and !orderIDs.empty()){
                Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                Quantity restingOrderQuantity = orderPtr->GetQuantity();
                if (quantity >= restingOrderQuantity){
                    orderPtr->Fill(restingOrderQuantity);
                    quantity -= restingOrderQuantity;
                    bestAskLevel.RemoveOrder(orderPtr);
                } else {
                    orderPtr->Fill(quantity);
                    quantity = 0;
                }
            }
        }
    }
    
    void LimitMatch(Side side, Quantity quantity, Price price){
        Price ask = orderBook.BestAsk();
        if (price < ask or orderBook.AsksEmpty()){
            orderBook.bids.at(price).AddOrder(order.get());
        } else {
            while (quantity > 0){
                PriceLevel& bestAskLevel = orderBook.BestAskLevel();
                if (bestAskLevel.GetPrice() < price){
                    OrderIDs& orderIDs = bestAskLevel.GetOrderIDs();
                    while (quantity > 0 and !orderIDs.empty()){
                        Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                        Quantity restingOrderQuantity = orderPtr->GetQuantity();
                        if (quantity >= restingOrderQuantity){
                            orderPtr->Fill(restingOrderQuantity);
                            quantity -= restingOrderQuantity;
                            bestAskLevel.RemoveOrder(orderPtr);
                        } else {
                            orderPtr->Fill(quantity);
                            quantity = 0;
                        }
                    }
                } else {
                    orderBook.bids.at(price).AddOrder(order.get());
                }
            }
        }
    }
    
    void printbook(){
        orderBook.PrintOrderBook();
    }
};
