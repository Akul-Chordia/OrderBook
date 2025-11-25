//
//  agents.h
//  OrderBook
//
class Agent {
protected:
    int agentID;
    int counter = 0;
    Quantity position = 0;
    Price PnL = 0;
    size_t lastTradeIndexProcessed = 0;
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
    
    inline int GetAgentIDFromOrderID(OrderID orderID) {
        return (orderID >> 52);
    }
    
    void UpdatePositionFromTrade(const Trade& trade) {
        Price tradePrice = trade.GetSpotPrice();
        Quantity tradeQuantity = trade.GetQuantity();
        
        if (GetAgentIDFromOrderID(trade.GetAggressorOrderID()) == agentID) {
            if (trade.GetSide() == Side::Buy) {
                position += tradeQuantity;
                PnL -= (static_cast<double>(tradeQuantity) * tradePrice);
            } else {
                position -= tradeQuantity;
                PnL += (static_cast<double>(tradeQuantity) * tradePrice);
            }
        } else if (GetAgentIDFromOrderID(trade.GetSittingOrderID()) == agentID) {
            if (trade.GetSide() == Side::Buy) {
                position -= tradeQuantity;
                PnL += (static_cast<double>(tradeQuantity) * tradePrice);
            } else {
                position += tradeQuantity;
                PnL -= (static_cast<double>(tradeQuantity) * tradePrice);
            }
        }
    }
        
    void ProcessNewTrades() {
        size_t currentTradeCount = trades.GetTradeCount();
        if (currentTradeCount > lastTradeIndexProcessed) {
            for (size_t i = lastTradeIndexProcessed; i < currentTradeCount; ++i) {
                try {
                    Trade trade = trades.GetTrade(i);
                    UpdatePositionFromTrade(trade);
                } catch (const std::out_of_range& e) {
                    throw std::logic_error("Trades mismatch");
                    break;
                }
            }
            lastTradeIndexProcessed = currentTradeCount;
        }
    }
    
    Price GetPnL(Price currentSpot){
        return (PnL + position*currentSpot);
    }
    
};


class RetailAgent : public Agent{
public:
    using Agent::Agent;
private:
    void run() {
        std::uniform_int_distribution<int> qty_dist(1, 20);
        std::normal_distribution<double> price_offset_dist(0, 4.0);
        const Price tickSize = 5;
        Price referencePrice = 10000;
        
        while (flag) {
            ProcessNewTrades();
            referencePrice = trades.GetLastSpotPrice();
            Quantity quantity = qty_dist(rng);
            Side side = (!(rng() % 2 == 0)) ? Side::Buy : Side::Sell;
            Price price = referencePrice+price_offset_dist(rng)*100.0;
            price = RoundToTick(price, tickSize);
            OrderID order_id = GenerateOrderID();
            counter++;
            auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            auto command = std::make_unique<Command>(order);
            gateway.Push(std::move(command));

//            std::cout << std::left << std::setw(8) << "Retail"
//                      << "ID: " << std::right << std::setw(5) << agentID
//                      << " | Pos: " << std::right << std::setw(5) << position
//                      << " | PnL: " << std::right << std::setw(12) << std::fixed << std::setprecision(2) << GetPnL(trades.GetLastSpotPrice())
//                      << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 1000 + 500));
        }
    }
};

class HFTAgent : public Agent{
private:
    OrderID lastBidID = 0;
    OrderID lastAskID = 0;
    std::vector<Price> prices;
    std::vector<double> logReturns;
    
    // Realistic HFT parameters
    const double maxLossLimit = -100000.0;     // Tighter stop-loss
    const Quantity maxPosition = 200;          // Smaller inventory limits
    const double maxAllowedVolatility = 80.0; // Pull quotes at high vol
    
    // Spread management (in ticks)
    const double baseHalfSpread = 0.5;         // 1 tick total spread (very tight)
    const double volSpreadAggression = 0.8;    // Widen by 0.8 ticks per unit vol
    
    // Inventory management
    const double inventorySkewAggression = 0.1; // Skew 10 ticks per 100 contracts
    
    // Trend following
    const double trendAggression = 1.5;         // Moderate trend following
    
    // Cold start handling
    size_t minTradesForStartup = 500;
    
    std::pair<double, double> CalculateMarketState() {
        trades.GetLastSpotPrices(500, prices);
        
        if (prices.size() < 2) return {0, 0};
        
        
        logReturns.reserve(prices.size() - 1);
        for (size_t i = 1; i < prices.size(); ++i) {
            if (prices[i-1] == 0) continue;
            double ratio = static_cast<double>(prices[i]) / static_cast<double>(prices[i-1]);
            if (ratio <= 0) continue;
            double ret = std::log(ratio);
            logReturns.push_back(ret);
        }
        
        if (logReturns.size() < 2) return {0, 0};
        
        double sum = 0.0;
        for (auto r : logReturns) sum += r;
        double mean = sum / logReturns.size();
        
        double varSum = 0.0;
        for (auto r : logReturns) {
            double diff = r - mean;
            varSum += diff * diff;
        }
        double variance = varSum / (logReturns.size() - 1);
        double volatility = std::sqrt(variance);
        
        if (prices.size() < minTradesForStartup) {
            volatility *= 0.1;
        }
        return {volatility * 10000, mean * 10000};
    }
    
    void CancelOldOrders() {
        if (lastBidID != 0) {
            gateway.Push(std::make_unique<Command>(lastBidID));
            lastBidID = 0;
        }
        if (lastAskID != 0) {
            gateway.Push(std::make_unique<Command>(lastAskID));
            lastAskID = 0;
        }
    }
    
public:
    using Agent::Agent;

private:
    void run() {
        std::uniform_int_distribution<int> qty_dist(3, 8);
        
        prices.reserve(500);
        logReturns.reserve(500);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        while (flag) {
            ProcessNewTrades();
            
            if (orderBook.BidsEmpty() || orderBook.AsksEmpty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }
            
            CancelOldOrders();
            
            auto [volatility, trend] = CalculateMarketState();
            double currentPnL = GetPnL(trades.GetLastSpotPrice());
            
            // PnL Stop-Loss
            if (currentPnL < maxLossLimit) {
                std::cout << "HFT     ID: " << std::setw(5) << agentID
                         << " | STOPPED (PnL): " << currentPnL << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            
            // Volatility circuit breaker
            if (volatility > maxAllowedVolatility) {
                std::cout << "HFT     ID: " << std::setw(5) << agentID
                         << " | PULLED (VOL): " << volatility << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            Price bestBid = orderBook.BestBid();
            Price bestAsk = orderBook.BestAsk();
            Price midPrice = (bestBid + bestAsk) / 2;
            
            // Calculate fair price with inventory skew and trend
            double inventorySkew = -position * inventorySkewAggression;
            double trendSkew = trend * trendAggression;
            Price ourFairPrice = midPrice + static_cast<Price>(inventorySkew + trendSkew);
            
            // Dynamic spread based on volatility
            double volSpread = volatility * volSpreadAggression;
            double halfSpread = baseHalfSpread + volSpread;
            
            // Final quotes
            Price newBidPrice = ourFairPrice - static_cast<Price>(halfSpread);
            Price newAskPrice = ourFairPrice + static_cast<Price>(halfSpread);
            
            // Safety checks
            if (newBidPrice >= newAskPrice || newBidPrice >= bestAsk || newAskPrice <= bestBid) {
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
                continue;
            }

            Quantity quantity = qty_dist(rng);
            
            // Quote only if within position limits
            if (position < maxPosition) {
                lastBidID = GenerateOrderID();
                counter++;
                auto bidOrder = std::make_unique<Order>(lastBidID, newBidPrice, quantity, OrderType::Limit, Side::Buy);
                gateway.Push(std::make_unique<Command>(bidOrder));
            }
            
            if (position > -maxPosition) {
                lastAskID = GenerateOrderID();
                counter++;
                auto askOrder = std::make_unique<Order>(lastAskID, newAskPrice, quantity, OrderType::Limit, Side::Sell);
                gateway.Push(std::make_unique<Command>(askOrder));
            }
            
//            std::cout << std::left << std::setw(8) << "HFT"
//                     << "ID: " << std::right << std::setw(5) << agentID
//                     << " | Pos: " << std::right << std::setw(5) << position
//                     << " | PnL: " << std::right << std::setw(12) << std::fixed
//                     << std::setprecision(2) << currentPnL
//                     << " | Vol: " << std::right << std::setw(5) << static_cast<int>(volatility)
//                     << " | Trend: " << std::right << std::setw(6) << std::fixed
//                     << std::setprecision(1) << trend
//                     << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 80 + 40));
        }
    }
};

class TWAPAgent : public Agent {
private:
    Quantity totalQuantityToExecute = 50000;
    std::chrono::seconds totalDuration = std::chrono::minutes(1);
    int numOrders = 60;
    Side side = Side::Sell;

public:
    using Agent::Agent;
private:
    void run() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000 + rng() % 5000));
        if (numOrders == 0) return;
        
        Quantity quantityPerOrder = totalQuantityToExecute / numOrders;
        if (quantityPerOrder == 0) return;
        
        auto timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(totalDuration) / numOrders;
        if (timeInterval.count() == 0) return;

        for (int i = 0; i < numOrders && flag; ++i) {
            ProcessNewTrades();
            OrderID order_id = GenerateOrderID();
            counter++;
            auto order = std::make_unique<Order>(order_id, quantityPerOrder, OrderType::Market, side);
            gateway.Push(std::make_unique<Command>(order));
            
           
//            std::cout << std::left << std::setw(8) << "TWAP"
//                      << "ID: " << std::right << std::setw(5) << agentID
//                      << " | Pos: " << std::right << std::setw(5) << position
//                      << " | PnL: " << std::right << std::setw(12) << std::fixed << std::setprecision(2) << GetPnL(trades.GetLastSpotPrice())
//                      << std::endl;
            std::this_thread::sleep_for(timeInterval + std::chrono::milliseconds(rng() % 80));
        }
    }
};

class MarketMakerAgent : public Agent{
private:
    OrderID lastBidID = 0;
    OrderID lastAskID = 0;
    
    const Quantity maxPosition = 500;
    const Price minSpreadToJoin = 3;
    const double inventoryPenalty = 0.02;
    
    void CancelOldOrders() {
        if (lastBidID != 0) {
            gateway.Push(std::make_unique<Command>(lastBidID));
            lastBidID = 0;
        }
        if (lastAskID != 0) {
            gateway.Push(std::make_unique<Command>(lastAskID));
            lastAskID = 0;
        }
    }
    
public:
    using Agent::Agent;

private:
    void run() {
        std::uniform_int_distribution<int> qty_dist(8, 15);

        while (flag) {
            ProcessNewTrades();
            
            if (orderBook.BidsEmpty() || orderBook.AsksEmpty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            Price bestBid = orderBook.BestBid();
            Price bestAsk = orderBook.BestAsk();
            Price spread = bestAsk - bestBid;
            
            CancelOldOrders();
            
            
            if (spread > minSpreadToJoin) {
                Price newBidPrice = bestBid + 1;
                Price newAskPrice = bestAsk - 1;
                
                Quantity baseQty = qty_dist(rng);
                double inventoryFactor = 1.0 - std::abs(position) * inventoryPenalty / maxPosition;
                inventoryFactor = std::max(0.3, std::min(1.0, inventoryFactor));
                Quantity adjustedQty = static_cast<Quantity>(baseQty * inventoryFactor);
                adjustedQty = std::max(static_cast<Quantity>(1), adjustedQty);
                
                if (position < maxPosition) {
                    lastBidID = GenerateOrderID();
                    counter++;
                    auto bidOrder = std::make_unique<Order>(lastBidID, newBidPrice, adjustedQty, OrderType::Limit, Side::Buy);
                    gateway.Push(std::make_unique<Command>(bidOrder));
                }
                
                if (position > -maxPosition) {
                    lastAskID = GenerateOrderID();
                    counter++;
                    auto askOrder = std::make_unique<Order>(lastAskID, newAskPrice, adjustedQty, OrderType::Limit, Side::Sell);
                    gateway.Push(std::make_unique<Command>(askOrder));
                }
            }
            
            std::cout << std::left << std::setw(8) << "MM"
                      << "ID: " << std::right << std::setw(5) << agentID
                      << " | Pos: " << std::right << std::setw(5) << position
                      << " | PnL: " << std::right << std::setw(12) << std::fixed << std::setprecision(2) << GetPnL(trades.GetLastSpotPrice())
                      << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 100));
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
private:
    const int lookbackPeriod = 100;
    const double momentumThreshold = 50.0;
    const Quantity maxPosition = 150;
    const double stopLoss = -20000.0;
    std::vector<Price> prices;
    std::vector<Price> RSIcache;
    
    enum class TrendState { NEUTRAL, UPTREND, DOWNTREND };
    TrendState currentTrend = TrendState::NEUTRAL;
    
    double CalculateMomentum() {
        trades.GetLastSpotPrices(lookbackPeriod, prices);
        
        if (prices.size() < lookbackPeriod) return 0.0;
        
        double sum = 0.0;
        for (size_t i = 0; i < prices.size() - 1; ++i) {
            sum += prices[i];
        }
        double avgPrice = sum / (prices.size() - 1);
        double currentPrice = prices.back();
        
        return currentPrice - avgPrice;
    }
    
    double CalculateRSI(int period = 14) {
        trades.GetLastSpotPrices(period + 1, RSIcache);
        
        if (RSIcache.size() < period + 1) return 50.0;
        
        double gains = 0.0;
        double losses = 0.0;
        
        for (size_t i = 1; i < RSIcache.size(); ++i) {
            double change = RSIcache[i] - RSIcache[i-1];
            if (change > 0) gains += change;
            else losses -= change;
        }
        
        if (losses == 0) return 100.0;
        
        double avgGain = gains / period;
        double avgLoss = losses / period;
        double rs = avgGain / avgLoss;
        
        return 100.0 - (100.0 / (1.0 + rs));
    }
    
public:
    using Agent::Agent;
    
private:
    void run() {
        std::uniform_int_distribution<int> qty_dist(10, 30);
        
        prices.reserve(lookbackPeriod);
        RSIcache.reserve(14);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        
        while (flag) {
            ProcessNewTrades();
            
            if (orderBook.BidsEmpty() || orderBook.AsksEmpty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            double momentum = CalculateMomentum();
            double rsi = CalculateRSI();
            double currentPnL = GetPnL(trades.GetLastSpotPrice());
            
            // Stop loss check
            if (currentPnL < stopLoss) {
                std::cout << "MOMENTUM ID: " << std::setw(5) << agentID
                         << " | STOPPED (LOSS): " << currentPnL << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                continue;
            }
            
            Price bestBid = orderBook.BestBid();
            Price bestAsk = orderBook.BestAsk();
            
            // Detect trend changes
            if (momentum > momentumThreshold && rsi < 70) {
                currentTrend = TrendState::UPTREND;
            } else if (momentum < -momentumThreshold && rsi > 30) {
                currentTrend = TrendState::DOWNTREND;
            } else if (std::abs(momentum) < momentumThreshold / 2) {
                currentTrend = TrendState::NEUTRAL;
            }
            
            // Trade based on momentum
            Quantity orderSize = qty_dist(rng);
            
            if (currentTrend == TrendState::UPTREND && position < maxPosition) {
                // Buy on uptrend
                OrderID order_id = GenerateOrderID();
                counter++;
                auto order = std::make_unique<Order>(order_id, bestAsk, orderSize, OrderType::Limit, Side::Buy);
                gateway.Push(std::make_unique<Command>(order));
                
            } else if (currentTrend == TrendState::DOWNTREND && position > -maxPosition) {
                // Sell on downtrend
                OrderID order_id = GenerateOrderID();
                counter++;
                auto order = std::make_unique<Order>(order_id, bestBid, orderSize, OrderType::Limit, Side::Sell);
                gateway.Push(std::make_unique<Command>(order));
                
            } else if (currentTrend == TrendState::NEUTRAL && std::abs(position) > 0) {
                // Close positions when momentum fades
                Side exitSide = (position > 0) ? Side::Sell : Side::Buy;
                Price exitPrice = (position > 0) ? bestBid : bestAsk;
                Quantity exitSize = std::min(static_cast<Quantity>(std::abs(position)), orderSize);
                
                OrderID order_id = GenerateOrderID();
                counter++;
                auto order = std::make_unique<Order>(order_id, exitPrice, exitSize, OrderType::Limit, exitSide);
                gateway.Push(std::make_unique<Command>(order));
            }
            
//            std::cout << std::left << std::setw(8) << "Momentum"
//                      << "ID: " << std::right << std::setw(5) << agentID
//                      << " | Pos: " << std::right << std::setw(5) << position
//                      << " | PnL: " << std::right << std::setw(12) << std::fixed << std::setprecision(2) << GetPnL(trades.GetLastSpotPrice())
//                      << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 2000 + 1000));
        }
    }
};
