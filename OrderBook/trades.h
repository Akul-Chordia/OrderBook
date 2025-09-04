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
    {
        spotPrice = price;
    };
    
    void Print() const {
        if (side == Side::Buy){
            std::cout << agressorOrderID << " bought " << quantity << "@" << price << " on " << timestamp << " from " << sittingOrderID << "\n";
        } else {
            std::cout << agressorOrderID << " sold " << quantity << "@" << price << " on " << timestamp << " to " << sittingOrderID << "\n";
        }
    }
};

using Trades = std::vector<Trade>;
Trades trades;

void print_trades(const Trades& trades){
    for (const auto& trade : trades) {
        trade.Print();
    }
};

