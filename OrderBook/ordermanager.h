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

struct Trade{
    OrderID buySideOrderID;
    OrderID sellSideOrderID;
    Price price;
    Quantity quantity;
    TimeStamp timestamp;
    
    Trade(OrderID buySideOrderID, OrderID sellSideOrderID, Price price, Quantity quantity, TimeStamp timestamp)
    :buySideOrderID{buySideOrderID},
    sellSideOrderID{sellSideOrderID},
    price{price},
    quantity{quantity},
    timestamp{timestamp}
    {};
    
    void Print() const {
        std::cout << quantity << "@" << price << " on " << timestamp << " ( Buy order ID:" << buySideOrderID << " Sell order ID:" << sellSideOrderID << ")\n";
    }
};

using Trades = std::vector<Trade>;

void print_trades(const Trades& trades){
    for (const auto& trade : trades) {
        trade.Print();
    }
};
