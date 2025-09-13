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
    
    Price BestAsk() const {
        return asks.begin()->first;
    }
    
    Price BestBid() const {
        return bids.begin()->first;
    }
    
    bool AsksEmpty() const {
        return asks.empty();
    }
    
    bool BidsEmpty() const {
        return bids.empty();
    }
    
    void PrintSpread() const {
        if (AsksEmpty() or BidsEmpty()){
            std::cout << "===== ∞ bips =====\n";
        } else {
            std::cout << "===== " << (BestAsk()-BestBid()) << " bips =====\n";
        }
    }

    void PrintOrderBook() const {
        std::cout << "\n======= ORDER BOOK =======\n";
        std::cout << std::fixed << std::setprecision(2);
        
        auto start_it = asks.begin();
        std::advance(start_it, std::min(static_cast<size_t>(25), asks.size()));

        for (auto it = std::make_reverse_iterator(start_it); it != asks.rend(); ++it) {
            const auto& [price, level] = *it;
            std::cout << price/100.0f << " ";
            for (std::size_t i = 0; i < static_cast<std::size_t>(level.GetQuantity() / 4); ++i) {
                std::cout << "█";
            }
            std::cout << " " << level.GetQuantity() << "\n";
        }

        PrintSpread();
        
        int count = 0;
        for (const auto& [price, level] : bids) {
            if (count >= 25) {
                break;
            }
            std::cout << price/100.0f << " ";
            for (std::size_t i = 0; i < static_cast<std::size_t>(level.GetQuantity() / 4); ++i) {
                std::cout << "█";
            }
            std::cout << " " << level.GetQuantity() << "\n";
            count++;
        }
    }
};
