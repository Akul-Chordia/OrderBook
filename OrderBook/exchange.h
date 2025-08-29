//
//  exchange.h
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
    
//    void dummyAddOrder(std::unique_ptr<Order> order) {
//        Price price = order->GetPrice();
//        //Quantity quantity = order->GetQuantity();
//        Side side = order->GetSide();
//        
//        if (side == Side::Buy) {
//            auto it = orderBook.bids.find(price);
//            if (it == orderBook.bids.end()) {
//                orderBook.bids.emplace(price, PriceLevel(price));
//            }
//            orderBook.bids.at(price).AddOrder(order.get());
//        } else {
//            auto it = orderBook.asks.find(price);
//            if (it == orderBook.asks.end()) {
//                orderBook.asks.emplace(price, PriceLevel(price));
//            }
//            orderBook.asks.at(price).AddOrder(order.get());
//        }
//        
//        orderManager.AddOrder(std::move(order));
//    }
    
    void AddOrder(std::unique_ptr<Order> order) {
        Order* rawOrder = order.get();
        //OrderID orderID = rawOrder->GetOrderID();
        Price price = rawOrder->GetPrice();
        Quantity quantity = rawOrder->GetQuantity();
        Side side = rawOrder->GetSide();
        OrderType ordertype = rawOrder->GetOrderType();
        
        orderManager.AddOrder(std::move(order));
        
        switch (ordertype) {
            case OrderType::Limit:
                LimitMatch(rawOrder, side, quantity, price);
                break;
            case OrderType::Market:
                MarketMatch(rawOrder, side, quantity);
                break;
            case OrderType::FillOrKill:
                FillOrKillMatch(rawOrder, side, quantity, price);
                break;
            case OrderType::ImmediateOrCancel:
                ImmediateOrCancelMatch(rawOrder, side, quantity, price);
                break;
            case OrderType::INVALID:
                throw std::runtime_error("INVALID ORDER TYPE");
                break;
        }
    }
    
    void MarketMatch(Order* order, Side side, Quantity quantity){
        if (side == Side::Buy){
            while (quantity > 0 && !orderBook.AsksEmpty()){
                PriceLevel& bestAskLevel = orderBook.BestAskLevel();
                OrderIDs& orderIDs = bestAskLevel.GetOrderIDs();
                while (quantity > 0 && !orderIDs.empty()){
                    Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                    Quantity restingOrderQuantity = orderPtr->GetRemainingQuantity();
                    if (quantity >= restingOrderQuantity){
                        orderPtr->Fill(restingOrderQuantity);
                        order->Fill(restingOrderQuantity);
                        quantity -= restingOrderQuantity;
                        bestAskLevel.RemoveOrder(orderPtr);
                    } else {
                        orderPtr->Fill(quantity);
                        order->Fill(quantity);
                        quantity = 0;
                    }
                }
                if (bestAskLevel.Empty()){
                    orderBook.asks.erase(orderBook.asks.begin());
                }
            }
        } else { // Sell side
            while (quantity > 0 && !orderBook.BidsEmpty()){
                PriceLevel& bestBidLevel = orderBook.BestBidLevel();
                OrderIDs& orderIDs = bestBidLevel.GetOrderIDs();
                while (quantity > 0 && !orderIDs.empty()){
                    Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                    Quantity restingOrderQuantity = orderPtr->GetRemainingQuantity();
                    if (quantity >= restingOrderQuantity){
                        orderPtr->Fill(restingOrderQuantity);
                        order->Fill(restingOrderQuantity);
                        quantity -= restingOrderQuantity;
                        bestBidLevel.RemoveOrder(orderPtr);
                    } else {
                        orderPtr->Fill(quantity);
                        order->Fill(quantity);
                        quantity = 0;
                    }
                }
                if (bestBidLevel.Empty()){
                    orderBook.bids.erase(orderBook.bids.begin());
                }
            }
        }
        
        // Cancel remaining quantity if market order couldn't be fully filled
        //it should keep the order pending until there is more liqudity
        if (quantity > 0){
            order->Cancel();
        }
    }
    
    void LimitMatch(Order* order, Side side, Quantity quantity, Price price){
        if (side == Side::Buy){
            // Match against asks
            while (quantity > 0 && !orderBook.AsksEmpty()){
                PriceLevel& bestAskLevel = orderBook.BestAskLevel();
                if (bestAskLevel.GetPrice() <= price){
                    OrderIDs& orderIDs = bestAskLevel.GetOrderIDs();
                    while (quantity > 0 && !orderIDs.empty()){
                        Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                        Quantity restingOrderQuantity = orderPtr->GetRemainingQuantity();
                        if (quantity >= restingOrderQuantity){
                            orderPtr->Fill(restingOrderQuantity);
                            order->Fill(restingOrderQuantity);
                            quantity -= restingOrderQuantity;
                            bestAskLevel.RemoveOrder(orderPtr);
                            //std::cout << "order_matched_a";
                        } else {
                            orderPtr->Fill(quantity);
                            order->Fill(quantity);
                            quantity = 0;
                            //std::cout << "order_matched_b";
                        }
                    }
                    if (bestAskLevel.Empty()){
                        orderBook.asks.erase(orderBook.asks.begin());
                    }
                } else {
                    break;
                }
            }
            
            // Add remaining quantity to orderbook
            if (quantity > 0){
                auto it = orderBook.bids.find(price);
                if (it == orderBook.bids.end()){
                    orderBook.bids.emplace(price, PriceLevel(price));
                }
                orderBook.bids.at(price).AddOrder(order);
            }
        } else { // Sell side
            while (quantity > 0 && !orderBook.BidsEmpty()){
                PriceLevel& bestBidLevel = orderBook.BestBidLevel();
                if (bestBidLevel.GetPrice() >= price){
                    OrderIDs& orderIDs = bestBidLevel.GetOrderIDs();
                    while (quantity > 0 && !orderIDs.empty()){
                        Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                        Quantity restingOrderQuantity = orderPtr->GetRemainingQuantity();
                        if (quantity >= restingOrderQuantity){
                            orderPtr->Fill(restingOrderQuantity);
                            order->Fill(restingOrderQuantity);
                            quantity -= restingOrderQuantity;
                            bestBidLevel.RemoveOrder(orderPtr);
                        } else {
                            orderPtr->Fill(quantity);
                            order->Fill(quantity);
                            quantity = 0;
                        }
                    }
                    if (bestBidLevel.Empty()){
                        orderBook.bids.erase(orderBook.bids.begin());
                    }
                } else {
                    break;
                }
            }
            
            // Add remaining quantity to orderbook
            if (quantity > 0){
                auto it = orderBook.asks.find(price);
                if (it == orderBook.asks.end()){
                    orderBook.asks.emplace(price, PriceLevel(price));
                }
                orderBook.asks.at(price).AddOrder(order);
            }
        }
    }
    
    void FillOrKillMatch(Order* order, Side side, Quantity quantity, Price price){
        Quantity availableQuantity = 0;
        
        if (side == Side::Buy){
            // Check if we can fill the entire order
            for (auto& [askPrice, askLevel] : orderBook.asks){
                if (askPrice <= price){
                    availableQuantity += askLevel.GetQuantity();
                    if (availableQuantity >= quantity){
                        break;
                    }
                } else {
                    break;
                }
            }
            
            if (availableQuantity >= quantity){
                LimitMatch(order, side, quantity, price);
            } else {
                order->Cancel();
            }
        } else {
            // Check if we can fill the entire order
            for (auto& [bidPrice, bidLevel] : orderBook.bids){
                if (bidPrice >= price){
                    availableQuantity += bidLevel.GetQuantity();
                    if (availableQuantity >= quantity){
                        break;
                    }
                } else {
                    break;
                }
            }
            
            if (availableQuantity >= quantity){
                LimitMatch(order, side, quantity, price);
            } else {
                order->Cancel();
            }
        }
    }
    
    void ImmediateOrCancelMatch(Order* order, Side side, Quantity quantity, Price price){
        //Quantity originalQuantity = quantity;
        
        if (side == Side::Buy){
            while (quantity > 0 && !orderBook.AsksEmpty()){
                PriceLevel& bestAskLevel = orderBook.BestAskLevel();
                if (bestAskLevel.GetPrice() <= price){
                    OrderIDs& orderIDs = bestAskLevel.GetOrderIDs();
                    while (quantity > 0 && !orderIDs.empty()){
                        Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                        Quantity restingOrderQuantity = orderPtr->GetRemainingQuantity();
                        if (quantity >= restingOrderQuantity){
                            orderPtr->Fill(restingOrderQuantity);
                            order->Fill(restingOrderQuantity);
                            quantity -= restingOrderQuantity;
                            bestAskLevel.RemoveOrder(orderPtr);
                        } else {
                            orderPtr->Fill(quantity);
                            order->Fill(quantity);
                            quantity = 0;
                        }
                    }
                    if (bestAskLevel.Empty()){
                        orderBook.asks.erase(orderBook.asks.begin());
                    }
                } else {
                    break;
                }
            }
        } else {
            while (quantity > 0 && !orderBook.BidsEmpty()){
                PriceLevel& bestBidLevel = orderBook.BestBidLevel();
                if (bestBidLevel.GetPrice() >= price){
                    OrderIDs& orderIDs = bestBidLevel.GetOrderIDs();
                    while (quantity > 0 && !orderIDs.empty()){
                        Order* orderPtr = orderManager.GetOrder(orderIDs.front());
                        Quantity restingOrderQuantity = orderPtr->GetRemainingQuantity();
                        if (quantity >= restingOrderQuantity){
                            orderPtr->Fill(restingOrderQuantity);
                            order->Fill(restingOrderQuantity);
                            quantity -= restingOrderQuantity;
                            bestBidLevel.RemoveOrder(orderPtr);
                        } else {
                            orderPtr->Fill(quantity);
                            order->Fill(quantity);
                            quantity = 0;
                        }
                    }
                    if (bestBidLevel.Empty()){
                        orderBook.bids.erase(orderBook.bids.begin());
                    }
                } else {
                    break;
                }
            }
        }
        if (quantity > 0){
            order->Cancel();
        }
    }
    
    void CancelOrder(OrderID orderID){
        Order* order = orderManager.GetOrder(orderID);
        if (order){
            order->Cancel();
            Price price = order->GetPrice();
            Side side = order->GetSide();
            if (side == Side::Buy){
                auto it = orderBook.bids.find(price);
                if (it != orderBook.bids.end()){
                    it->second.RemoveOrder(order);
                    if (it->second.Empty()){
                        orderBook.bids.erase(it);
                    }
                }
            } else {
                auto it = orderBook.asks.find(price);
                if (it != orderBook.asks.end()){
                    it->second.RemoveOrder(order);
                    if (it->second.Empty()){
                        orderBook.asks.erase(it);
                    }
                }
            }
        }
    }
    
    void PrintBook(){
        orderBook.PrintOrderBook();
    }
};
