//
//  order.h
//  OrderBook
//
class Order{
private:
    OrderID orderID;
    Price price;
    Quantity quantity;
    Quantity remainingQuantity;
    OrderType orderType;
    Side side;
    TimeStamp timestamp;
    OrderStatus orderStatus;
    
public:
    Order(OrderID orderID, Price price, Quantity quantity, OrderType orderType, Side side, TimeStamp timestamp = std::chrono::steady_clock::now().time_since_epoch())
        :orderID{orderID},
        price{price},
        quantity{quantity},
        remainingQuantity{quantity},
        orderType{orderType},
        side{side},
        timestamp{timestamp},
        orderStatus{OrderStatus::New}
    {}
    
    Order(OrderID orderID, Quantity quantity, OrderType orderType, Side side, TimeStamp timestamp = std::chrono::steady_clock::now().time_since_epoch())
        :orderID{orderID},
        price{INVALID},
        quantity{quantity},
        remainingQuantity{quantity},
        orderType{orderType},
        side{side},
        timestamp{timestamp},
        orderStatus{OrderStatus::New}
    {}
    
    OrderID GetOrderID() const { return orderID; }
    Price GetPrice() const { return price; }
    Quantity GetQuantity() const { return quantity; }
    Quantity GetRemainingQuantity() const { return remainingQuantity; }
    Quantity GetFilledQuantity() const { return quantity - remainingQuantity; }
    OrderType GetOrderType() const { return orderType; }
    Side GetSide() const { return side; }
    TimeStamp GetTimestamp() const { return timestamp; }
    OrderStatus GetOrderStatus() const { return orderStatus; }
    
    
    void Fill(Quantity quantity){
        if (quantity > remainingQuantity){
            throw std::logic_error("Filling more than remaining quantity");  // this should never happen
        }
        remainingQuantity -= quantity;
        (remainingQuantity == 0) ? orderStatus = OrderStatus::Filled : orderStatus = OrderStatus::PartiallyFilled;
    }
    
    void Cancel(){
        orderStatus = OrderStatus::Cancelled;
    }
    
    void Rejected(){
        orderStatus = OrderStatus::Rejected;
    }
    
    void Modify(Price price = INVALID, Quantity quantity = INVALID, OrderType orderType = OrderType::INVALID, Side side = Side::INVALID){
        if (price != INVALID){ this->price = price; }
        if (quantity != INVALID){
            this->quantity = quantity;
            remainingQuantity = quantity;
        }
        if (orderType != OrderType::INVALID){ this->orderType = orderType; }
        if (side != Side::INVALID){ this->side = side; }
        timestamp = std::chrono::steady_clock::now().time_since_epoch();
        orderStatus = OrderStatus::New;
    }
};
