//
//  pricelevels.h
//  OrderBook
//

using OrderIDs = std::deque<OrderID>;
using OrderIDMap = std::unordered_map<OrderID, OrderIDs::iterator>;

class PriceLevel{
private:
    Price price;
    Quantity totalQuantity = 0;
    OrderIDs orderIDs;
    OrderIDMap orderIDMap;
public:
    explicit PriceLevel(Price price)
    :price(price)
    {}

    void AddOrder(OrderID orderID, Quantity quantity) {
        orderIDs.push_back(orderID);
        orderIDMap[orderID] = std::prev(orderIDs.end());
        totalQuantity += quantity;
    }

    void RemoveOrder(OrderID orderID, Quantity quantity) {
        auto it = orderIDMap.find(orderID);
        if (it != orderIDMap.end()) {
            orderIDs.erase(it->second);
            orderIDMap.erase(it);
            totalQuantity -= quantity;
        }
    }

    OrderIDs& GetOrderIDs() { return orderIDs; }
    Quantity GetTotalQuantity() const { return totalQuantity; }
};
