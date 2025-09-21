//
//  gateway.h
//  OrderBook
//
struct ModifyOrderPayload {
    OrderID orderID;
    Price newPrice;
    Quantity newQuantity;
};

struct Command {
    CommandType type;
    std::variant<OrderPtr, OrderID, ModifyOrderPayload> payload;
    
    explicit Command(OrderPtr& orderptr)
        :type(CommandType::PlaceOrder),
        payload(std::move(orderptr))
    {}

    explicit Command(OrderID orderID)
        :type(CommandType::CancelOrder),
        payload(orderID)
    {}
    
    explicit Command(OrderID orderID, Price price, Quantity quantity)
        : type(CommandType::ModifyOrder)
    {
        payload.emplace<ModifyOrderPayload>(orderID, price, quantity);
    }
};

using CommandPtr = std::unique_ptr<Command>;

class Gateway{
private:
    std::queue<CommandPtr> gateway;
    mutable std::mutex gateway_mutex;
    std::condition_variable cv;
public:
    Gateway() = default;
    Gateway(const Gateway&) = delete;
    Gateway& operator=(const Gateway&) = delete;
    
    void Push(CommandPtr command){
        std::lock_guard<std::mutex> lock(gateway_mutex);
//        std::cout << "\norder pushed : " << std::get<OrderPtr>(command->payload)->GetOrderID() << " " << std::get<OrderPtr>(command->payload)->GetPrice() << " " <<
//        static_cast<int>(std::get<OrderPtr>(command->payload)->GetSide()) << " " <<
//            std::get<OrderPtr>(command->payload)->GetQuantity() ;
        gateway.push(std::move(command));
        
        cv.notify_one();
    }
    
    bool WaitAndPop(CommandPtr& command){
        std::unique_lock<std::mutex> lock(gateway_mutex);
        if (cv.wait_for(lock, std::chrono::milliseconds(100), [this]{return !gateway.empty();})){
            command = std::move(gateway.front());
            gateway.pop();
            return true;
        } else {
            return false;
        }
    }
    
    bool IsEmpty(){
        if (gateway.empty()){
            return true;
        }
        return false;
    }
};
