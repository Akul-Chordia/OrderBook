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
        Price price = order->GetPrice();
        Quantity qty = order->GetQuantity();
        Side side = order->GetSide();
        OrderID id = order->GetOrderID();
        
        if (side == Side::Buy) {
            auto it = orderBook.bids.find(price);
            if (it == orderBook.bids.end()) {
                orderBook.bids.emplace(price, PriceLevel(price));
            }
            orderBook.bids.at(price).AddOrder(id, qty);
        } else {
            auto it = orderBook.asks.find(price);
            if (it == orderBook.asks.end()) {
                orderBook.asks.emplace(price, PriceLevel(price));
            }
            orderBook.asks.at(price).AddOrder(id, qty);
        }
        
        
        orderManager.AddOrder(std::move(order));
    }
    
    void AddOrder(std::unique_ptr<Order> order) {
        OrderID orderID = order->GetOrderID();
        Price price = order->GetPrice();
        Quantity quantity = order->GetQuantity();
        Side side = order->GetSide();
        
        switch (order->GetOrderType()) {
            case OrderType::Limit:
                // call efficient limit logic
                break;
            case OrderType::Market:
                // call market logic
                break;
            case OrderType::FillOrKill:
                // call FOK logic
                break;
            case OrderType::ImmediateOrCancel:
                // call IOC logic
                break;
            case OrderType::INVALID:
                std::cout << "this should never happen";
                break;
        }
    }
};
