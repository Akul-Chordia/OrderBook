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
    
    void PrintSpread(){
        if (AsksEmpty() or BidsEmpty()){
            std::cout << "===== ∞ bips =====\n";
        } else {
            std::cout << "===== " << (BestAsk()-BestBid()) << " bips =====\n";
        }
    }

    void PrintOrderBook() {
        std::cout << "\n======= ORDER BOOK =======\n";
        std::cout << std::fixed << std::setprecision(2);
        for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
            const auto& [price, level] = *it;
            std::cout << price/100.0f << " ";
            for (std::size_t i = 0; i < static_cast<std::size_t>(level.GetQuantity() / 4); ++i) {
                std::cout << "█";
            }
            std::cout << " " << level.GetQuantity() << "\n";
        }
        PrintSpread();
        for (const auto& [price, level] : bids) {
            std::cout << price/100.0f << " ";
            for (std::size_t i = 0; i < static_cast<std::size_t>(level.GetQuantity() / 4); ++i) {
                std::cout << "█";
            }
            std::cout << " " << level.GetQuantity() << "\n";
        }
    }
};
