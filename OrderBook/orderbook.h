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
    OrderManager& orderManager;

public:
    explicit OrderBook(OrderManager& orderManager)
    :orderManager(orderManager)
    {}

    void AddOrder(std::unique_ptr<Order> order) {
        Price price = order->GetPrice();
        Quantity qty = order->GetQuantity();
        Side side = order->GetSide();
        OrderID id = order->GetOrderID();

        if (side == Side::Buy) {
            auto it = bids.find(price);
            if (it == bids.end()) {
                bids.emplace(price, PriceLevel(price));
            }
            bids.at(price).AddOrder(id, qty);
        } else {
            auto it = asks.find(price);
            if (it == asks.end()) {
                asks.emplace(price, PriceLevel(price));
            }
            asks.at(price).AddOrder(id, qty);
        }


        orderManager.AddOrder(std::move(order));
    }
    
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
