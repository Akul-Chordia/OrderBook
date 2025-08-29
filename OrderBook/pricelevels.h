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
    
    Price GetPrice() const { return price;}
    Quantity GetQuantity() const { return totalQuantity;}
    OrderIDs& GetOrderIDs() { return orderIDs;}
    std::size_t GetOrderCount() const { return orderIDs.size(); }

    void AddOrder(Order* orderPtr) {
        OrderID orderID = orderPtr->GetOrderID();
        orderIDs.push_back(orderID);
        orderIDMap[orderID] = std::prev(orderIDs.end());
        totalQuantity += orderPtr->GetRemainingQuantity();
    }

    void RemoveOrder(Order* orderPtr) {
        auto it = orderIDMap.find(orderPtr->GetOrderID());
        if (it != orderIDMap.end()) {
            totalQuantity -= orderPtr->GetRemainingQuantity();
            orderIDs.erase(it->second);
            orderIDMap.erase(it);
        }
    }
    
    void PartialFill(Quantity quantity){
        totalQuantity -= quantity;
        if (totalQuantity <= 0){
            throw std::logic_error("Negative quantity");  // this should never happen
        }
    }
    
    bool Empty() const {
        return orderIDs.empty();
    }
};
