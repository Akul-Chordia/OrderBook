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
    void AddOrder(std::unique_ptr<Order> order) {
            OrderID orderID = order->GetOrderID();
            orders[orderID] = std::move(order);
    }

    inline Order* GetOrder(OrderID orderID) {
        auto it = orders.find(orderID);
        return (it != orders.end()) ? it->second.get() : nullptr;
    }

    bool ModifyOrder(OrderID orderID, Price price = INVALID, Quantity quantity = INVALID,
                     OrderType orderType = OrderType::INVALID, Side side = Side::INVALID) {
        Order* orderPtr = GetOrder(orderID);
        if (!orderPtr) return false;
        orderPtr->Modify(price, quantity, orderType, side);
        return true;
    }
};

