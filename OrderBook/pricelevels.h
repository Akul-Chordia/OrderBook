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
    
    Price GetPrice() { return price;}
    Quantity GetQuantity() const { return totalQuantity;}
    OrderIDs& GetOrderIDs() { return orderIDs;}

    void AddOrder(Order* orderPtr) {
        OrderID orderID = orderPtr->GetOrderID();
        orderIDs.push_back(orderID);
        orderIDMap[orderID] = std::prev(orderIDs.end());
        totalQuantity += orderPtr->GetQuantity();
    }

    void RemoveOrder(Order* orderPtr) {
        auto it = orderIDMap.find(orderPtr->GetOrderID());
        if (it != orderIDMap.end()) {
            orderIDs.erase(it->second);
            orderIDMap.erase(it);
            totalQuantity -= orderPtr->GetQuantity();
        }
    }
    
    bool Empty(){
        return orderIDs.empty();
    }
};
