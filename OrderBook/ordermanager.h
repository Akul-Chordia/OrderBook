//
//  ordermanager.h
//  OrderBook
//

using OrderPtr = std::unique_ptr<Order>;
using OrderPtrs = std::unordered_map<OrderID, OrderPtr>;


class OrderManager{
private:
    OrderPtrs orders;
public:
    void AddOrder(OrderPtr order) {
            OrderID orderID = order->GetOrderID();
            orders[orderID] = std::move(order);
    }

    inline Order* GetOrder(OrderID orderID) {
        auto it = orders.find(orderID);
        return (it != orders.end()) ? it->second.get() : nullptr;
    }
};

