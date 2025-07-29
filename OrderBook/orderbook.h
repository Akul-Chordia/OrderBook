//
//  orderbook.h
//  OrderBook
//
using Asks = std::map<Price, PriceLevel>;
using Bids  = std::map<Price, PriceLevel, std::greater<>>;

class OrderBook {
private:
    Asks asks;
    Bids bids;

public:
    friend class Exchange;
    
    Price BestAsk(){
        return asks.begin()->first;
    }
    
    Price BestBid(){
        return bids.begin()->first;
    }

    void PrintOrderBook() {
        std::cout << "Asks:\n";
        for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
            const auto& [price, level] = *it;
            std::cout << price << " " << std::string(static_cast<std::size_t>(level.GetTotalQuantity()), '|');
            std::cout << level.GetTotalQuantity() << "\n";
        }
        std::cout << "Bids:\n";
        for (const auto& [price, level] : bids) {
            std::cout << price << " " << std::string(static_cast<std::size_t>(level.GetTotalQuantity()), '|');
            std::cout << level.GetTotalQuantity() << "\n";
        }
    }
};
