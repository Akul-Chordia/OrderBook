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
        OrderID orderID = order->GetOrderID();
        Price price = order->GetPrice();
        Quantity quantity = order->GetQuantity();
        Side side = order->GetSide();
        
        orderManager.AddOrder(std::move(order));
        
        if (side == Side::Buy){

            
            switch (order->GetOrderType()) {
                case OrderType::Limit: {
                    //                    PriceLevel bestAsk = orderBook.BestAsk();
                    //                    Price ask = bestAsk.GetPrice();
                    //                    Quantity askQuantity = bestAsk.GetQuantity();
                    //
                    //                    if (price < ask){
                    //                        orderBook.bids.at(price).AddOrder(orderID, quantity);
                    //                    } else {
                    //                        //do logic
                    //                    }
                }
                    break;
                case OrderType::Market:
                    while (quantity > 0 and !orderBook.AsksEmpty()){
                        PriceLevel& bestAsk = orderBook.BestAsk();
                        OrderIDs& orderIDs = bestAsk.GetOrderIDs();
                        while (quantity > 0 and !orderIDs.empty()){
                            Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                            Quantity restingOrderQunatity = orderPtr->GetQuantity();
                            if (quantity >= restingOrderQunatity){
                                orderPtr->Fill(restingOrderQunatity);
                                quantity -= restingOrderQunatity;
                                bestAsk.RemoveOrder(orderPtr);
                            } else {
                                orderPtr->Fill(quantity);
                                quantity = 0;
                            }
                        }
                    }
                    
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
        } else {
            PriceLevel bestBid = orderBook.BestBid();
            Price bid = bestBid.GetPrice();
            Quantity bidQuantity = bestBid.GetQuantity();
        }
        


        

    }
    
    void printbook(){
        orderBook.PrintOrderBook();
    }
};
