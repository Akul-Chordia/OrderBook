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
    
    PriceLevel& BestAskLevel(){
        return asks.begin()->second;
    }
    
    PriceLevel& BestBidLevel(){
        return bids.begin()->second;
    }
    
    Price BestAsk(){
        return asks.begin()->first;
    }
    
    Price BestBid(){
        return bids.begin()->first;
    }
    
    bool AsksEmpty(){
        return asks.empty();
    }
    
    bool BidsEmpty(){
        return bids.empty();
    }

    void PrintOrderBook() {
        std::cout << "\n======= ORDER BOOK =======\n";
        std::cout << "Asks:\n";
        for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
            const auto& [price, level] = *it;
            std::cout << price/100.0f << " " << std::string(static_cast<std::size_t>(level.GetQuantity()/2), '|');
            std::cout << level.GetQuantity() << "\n";
        }
        std::cout << "===== " << (BestAsk()-BestBid()) << " bips =====\n";
        std::cout << "Bids:\n";
        for (const auto& [price, level] : bids) {
            std::cout << price/100.0f << " " << std::string(static_cast<std::size_t>(level.GetQuantity()/2), '|');
            std::cout << level.GetQuantity() << "\n";
        }
    }
};
