//
//  trades.h
//  OrderBook
//
class Trade{
private:
    OrderID agressorOrderID;
    OrderID sittingOrderID;
    Side side;
    Price price;
    Quantity quantity;
    TimeStamp timestamp;
    
public:
    Trade(OrderID agressorOrderID, OrderID sittingOrderID, Side side, Price price, Quantity quantity, TimeStamp timestamp = std::chrono::steady_clock::now().time_since_epoch())
    :agressorOrderID{agressorOrderID},
    sittingOrderID{sittingOrderID},
    side{side},
    price{price},
    quantity{quantity},
    timestamp{timestamp}
    {};
    
    const Price GetSpotPrice() const {
        return price;
    }
    
    OrderID GetAggressorOrderID() const { return agressorOrderID; }
    OrderID GetSittingOrderID() const { return sittingOrderID; }
    Side GetSide() const { return side; }
    Quantity GetQuantity() const { return quantity; }
    
    void Print() const {
        if (side == Side::Buy){
            std::cout << agressorOrderID << " bought " << quantity << "@" << price << " on " << timestamp << " from " << sittingOrderID << "\n";
        } else {
            std::cout << agressorOrderID << " sold " << quantity << "@" << price << " on " << timestamp << " to " << sittingOrderID << "\n";
        }
    }
};

class Trades {
private:
    std::vector<Trade> trades;
    mutable std::mutex writer_mutex;
    std::atomic<size_t> trade_count;

public:
    Trades()
    :trade_count{0}
    {
        trades.reserve(1000000); // Reserve space for 1 million trades
    }

    Trades(const Trades&) = delete;
    Trades& operator=(const Trades&) = delete;

    void AddTrade(OrderID agressorOrderID, OrderID sittingOrderID, Side side, Price price, Quantity quantity, TimeStamp timestamp = std::chrono::steady_clock::now().time_since_epoch()) {
        std::lock_guard<std::mutex> lock(writer_mutex);
        trades.emplace_back(agressorOrderID, sittingOrderID, side, price, quantity, timestamp);
        trade_count.store(trades.size(), std::memory_order_release);
    }


    Price GetLastSpotPrice() const {

        size_t current_count = trade_count.load(std::memory_order_acquire);
        
        if (current_count == 0) {
            return 10000;
        }
        return trades[current_count - 1].GetSpotPrice();
    }

    std::vector<Price> GetLastSpotPrices(size_t count) const {
        size_t current_count = trade_count.load(std::memory_order_acquire);
        
        std::vector<Price> prices;
        if (current_count == 0) {
            return prices;
        }

        size_t start = (current_count > count) ? (current_count - count) : 0;
        prices.reserve(current_count - start);
        
        for (size_t i = start; i < current_count; ++i) {
            prices.push_back(trades[i].GetSpotPrice());
        }
        return prices;
    }

    size_t GetTradeCount() const {
        return trade_count.load(std::memory_order_acquire);
    }

    Trade GetTrade(size_t index) const {
        size_t current_count = trade_count.load(std::memory_order_acquire);
        
        if (index >= current_count) {
            throw std::out_of_range("Trade index out of range. (Attempted to read a trade that hasn't been published yet)");
        }
        return trades[index];
    }

    void PrintTrades() const {
        size_t current_count = trade_count.load(std::memory_order_acquire);
        
        for (size_t i = 0; i < current_count; ++i) {
            trades[i].Print();
        }
    }
    
    bool IsEmpty() const {
        return trade_count.load(std::memory_order_acquire) == 0;
    }
};
