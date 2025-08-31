//
//  gateway.h
//  OrderBook
//
struct Command {
    CommandType type;
    std::variant<OrderPtr, OrderID> payload;
    
    explicit Command(OrderPtr& orderptr)
        :type(CommandType::PlaceOrder),
        payload(std::move(orderptr))
    {}

    explicit Command(OrderID orderID)
        :type(CommandType::CancelOrder),
        payload(orderID)
    {}
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
        gateway.push(std::move(command));
        cv.notify_one();
    }
    
    void WaitAndPop(CommandPtr& command){
        std::unique_lock<std::mutex> lock(gateway_mutex);
        cv.wait(lock, [this] { return !gateway.empty(); });
        command = std::move(gateway.front());
        gateway.pop();
    }
    
    bool IsEmpty(){
        if (gateway.empty()){
            return true;
        }
        return false;
    }
};
