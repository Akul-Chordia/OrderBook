//
//  agents.h
//  OrderBook
//
class Agent {
protected:
    int agentID;
    int counter = 0;
    Gateway& gateway;
    const OrderBook& orderBook;
    const Trades& trades;
    std::thread agent_thread;
    std::atomic<bool>& flag;
    std::mt19937 rng;
public:
    Agent(int agentID, Gateway& gateway, const OrderBook& orderBook, const Trades& trades, std::atomic<bool>& flag)
    :agentID{agentID},
    gateway{gateway},
    orderBook{orderBook},
    trades{trades},
    flag{flag},
    rng(static_cast<unsigned>(agentID) + std::chrono::high_resolution_clock::now().time_since_epoch().count())
    {}

    Agent(const Agent&) = delete;
    Agent& operator=(const Agent&) = delete;
    
    virtual ~Agent() {
        join();
    }

    void start() {
       agent_thread = std::thread(&Agent::run, this);
    }

    void join() {
       if (agent_thread.joinable()) {
           agent_thread.join();
       }
    }

protected:

    virtual void run() = 0;
    
    OrderID GenerateOrderID(){
        return ((static_cast<OrderID>(agentID) << 52) | counter);
    }
    
    Price RoundToTick(Price price, Price tickSize) {
        return std::round(static_cast<double>(price) / tickSize) * tickSize;
    }
};


class RetailAgent : public Agent{
public:
    using Agent::Agent;
private:
    void run() {
        // Each thread needs its own random number generator
        std::uniform_int_distribution<int> qty_dist(1, 20);
        std::normal_distribution<double> price_offset_dist(0, 4.0);
        const Price tickSize = 5;

        
        while (flag) {
            Price referencePrice = 10000;
            if (!orderBook.BidsEmpty() && !orderBook.AsksEmpty()) {
                referencePrice = orderBook.BestBid() + (orderBook.BestAsk() - orderBook.BestBid()) / 2;
            } else if (!orderBook.BidsEmpty()) {
                referencePrice = orderBook.BestBid();
            } else if (!orderBook.AsksEmpty()) {
                referencePrice = orderBook.BestAsk();
            }
            Quantity quantity = qty_dist(rng);
            Side side = (!(rng() % 2 == 0)) ? Side::Buy : Side::Sell;
            Price price = referencePrice+price_offset_dist(rng)*100.0;
            price = RoundToTick(price, tickSize);
            OrderID order_id = GenerateOrderID();
            counter++;
            auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            auto command = std::make_unique<Command>(order);
            gateway.Push(std::move(command));

            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 1000 + 500));
        }
    }
};

class HFTAgent : public Agent{
private:
    OrderID lastBidID = 0;
    OrderID lastAskID = 0;
public:
    using Agent::Agent;
private:
    void run() {
        std::uniform_int_distribution<int> qty_dist(50, 100);

        while (flag) {
            if (!orderBook.BidsEmpty() && !orderBook.AsksEmpty()) {
                Price bestBid = orderBook.BestBid();
                Price bestAsk = orderBook.BestAsk();
                
                if (lastBidID != 0) {
                    auto command = std::make_unique<Command>(lastBidID);
                    gateway.Push(std::move(command));
                }
                if (lastAskID != 0) {
                    auto command = std::make_unique<Command>(lastAskID);
                    gateway.Push(std::move(command));
                }
                
                Price newBidPrice = bestBid + 1;
                Price newAskPrice = bestAsk - 1;
                Quantity quantity = qty_dist(rng);

                lastBidID = GenerateOrderID();
                counter++;
                auto bidOrder = std::make_unique<Order>(lastBidID, newBidPrice, quantity, OrderType::Limit, Side::Buy);
                auto bidCommand = std::make_unique<Command>(bidOrder);
                gateway.Push(std::move(bidCommand));
                
                lastAskID = GenerateOrderID();
                counter++;
                auto askOrder = std::make_unique<Order>(lastAskID, newAskPrice, quantity, OrderType::Limit, Side::Sell);
                auto askCommand = std::make_unique<Command>(askOrder);
                gateway.Push(std::move(askCommand));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 80 + 40));
        }
    }
};

class TWAPAgent : public Agent {
private:
    Quantity totalQuantityToExecute = 100000;
    std::chrono::seconds totalDuration = std::chrono::minutes(5);
    int numOrders = 200;

public:
    using Agent::Agent;
private:
    void run() {
        if (numOrders == 0) return;
        
        Quantity quantityPerOrder = totalQuantityToExecute / numOrders;
        if (quantityPerOrder == 0) return;
        
        auto timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(totalDuration) / numOrders;
        if (timeInterval.count() == 0) return;

        for (int i = 0; i < numOrders; ++i) {
            if (!flag) break;
            OrderID order_id = GenerateOrderID();
            counter++;
            auto order = std::make_unique<Order>(order_id, quantityPerOrder, OrderType::Market, Side::Buy);
            gateway.Push(std::make_unique<Command>(order));

            std::this_thread::sleep_for(timeInterval);
        }
    }
};


class VWAPAgent : public Agent{
public:
    using Agent::Agent;
private:
    void run() {
        while (flag) {
            // Generate random order details
//            Quantity quantity = qty_dist(rng);
//            Side side = (!(rng() % 2 == 0)) ? Side::Buy : Side::Sell;
//            Price price = (rng() % 2 == 0) ? price_dist1(rng)*100.0 : price_dist2(rng)*100.0;
//            OrderID order_id = GenerateOrderID();
//            counter++;
            //auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            //auto command = std::make_unique<Command>(order);
            //gateway.Push(std::move(command));

//            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 50));
        }
    }
};


class MomentumAgent : public Agent{
public:
    using Agent::Agent;
private:
    void run() {
        while (flag) {
            // Generate random order details
//            Quantity quantity = qty_dist(rng);
//            Side side = (!(rng() % 2 == 0)) ? Side::Buy : Side::Sell;
//            Price price = (rng() % 2 == 0) ? price_dist1(rng)*100.0 : price_dist2(rng)*100.0;
//            OrderID order_id = GenerateOrderID();
//            counter++;
            //auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            //auto command = std::make_unique<Command>(order);
            //gateway.Push(std::move(command));

//            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 50));
        }
    }
};


