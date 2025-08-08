//
//  trades.h
//  OrderBook
//
class Trade{
private:
    OrderID buySideOrderID;
    OrderID sellSideOrderID;
    Price price;
    Quantity quantity;
    TimeStamp timestamp;
    
public:
    Trade(OrderID buySideOrderID, OrderID sellSideOrderID, Price price, Quantity quantity, TimeStamp timestamp)
    :buySideOrderID{buySideOrderID},
    sellSideOrderID{sellSideOrderID},
    price{price},
    quantity{quantity},
    timestamp{timestamp}
    {};
    
    void Print() const {
        std::cout << quantity << "@" << price << " on " << timestamp << " ( Buy order ID:" << buySideOrderID << " Sell order ID:" << sellSideOrderID << ")\n";
    }
};

using Trades = std::vector<Trade>;

void print_trades(const Trades& trades){
    for (const auto& trade : trades) {
        trade.Print();
    }
};

