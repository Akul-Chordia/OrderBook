//
//  agents.h
//  OrderBook
//
class RandomAgent : public Agent{
public:
    using Agent::Agent;
private:
    void run() {
        // Each thread needs its own random number generator
        std::uniform_int_distribution<int> qty_dist(1, 20);
        std::normal_distribution<double> price_dist1(99, 1.0);
        std::normal_distribution<double> price_dist2(101, 1.0);

        
        while (flag) {
            // Generate random order details
            Quantity quantity = qty_dist(rng);
            Side side = (!(rng() % 2 == 0)) ? Side::Buy : Side::Sell;
            Price price = (rng() % 2 == 0) ? price_dist1(rng)*100.0 : price_dist2(rng)*100.0;
            OrderID order_id = GenerateOrderID();
            counter++;
            auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            auto command = std::make_unique<Command>(order);
            gateway.Push(std::move(command));

            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 50));
        }
    }
};

class HFTAgent : public Agent{
public:
    using Agent::Agent;
private:
    void run() {
        while (flag) {
            // Generate random order details
//            Quantity quantity = qty_dist(rng);
//            Side side = (!(rng() % 2 == 0)) ? Side::Buy : Side::Sell;
//            Price price = (rng() % 2 == 0) ? price_dist1(rng)*100.0 : price_dist2(rng)*100.0;
            OrderID order_id = GenerateOrderID();
            counter++;
            //auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            //auto command = std::make_unique<Command>(order);
            //gateway.Push(std::move(command));

            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 50));
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
            OrderID order_id = GenerateOrderID();
            counter++;
            //auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            //auto command = std::make_unique<Command>(order);
            //gateway.Push(std::move(command));

            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 50));
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
            OrderID order_id = GenerateOrderID();
            counter++;
            //auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            //auto command = std::make_unique<Command>(order);
            //gateway.Push(std::move(command));

            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 50));
        }
    }
};

class RetailAgent : public Agent{
public:
    using Agent::Agent;
private:
    void run() {
        while (flag) {
            // Generate random order details
//            Quantity quantity = qty_dist(rng);
//            Side side = (!(rng() % 2 == 0)) ? Side::Buy : Side::Sell;
//            Price price = (rng() % 2 == 0) ? price_dist1(rng)*100.0 : price_dist2(rng)*100.0;
            OrderID order_id = GenerateOrderID();
            counter++;
            //auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            //auto command = std::make_unique<Command>(order);
            //gateway.Push(std::move(command));

            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 50));
        }
    }
};

