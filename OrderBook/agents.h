//
//  agents.h
//  OrderBook
//

class Agent {
private:
    int agentID;
    int counter = 0;
    Gateway& gateway;
    std::thread agent_thread;
    std::atomic<bool>& flag;
public:
    Agent(int agentID, Gateway& gateway, std::atomic<bool>& flag)
        :agentID(agentID),
        gateway(gateway),
        flag(flag)
    {}

    Agent(const Agent&) = delete;
    Agent& operator=(const Agent&) = delete;


    void start() {
       agent_thread = std::thread(&Agent::run, this);
    }

    void join() {
       if (agent_thread.joinable()) {
           agent_thread.join();
       }
    }

private:

    void run() {
        // Each thread needs its own random number generator
        std::mt19937 rng(static_cast<unsigned>(agentID) + std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> qty_dist(1, 20);
        std::uniform_real_distribution<double> price_dist(90.0, 105.0);
        std::uniform_real_distribution<double> price_dist2(95.0, 110.0);
        
        while (flag) {
            // Generate random order details
            //Price price = std::round(price_dist(rng) * 100.0/100);
            Quantity quantity = qty_dist(rng);
            Side side = (!(rng() % 2 == 0)) ? Side::Buy : Side::Sell;
            Price price = (rng() % 2 == 0) ? std::round(price_dist(rng))*100 : std::round(price_dist2(rng))*100;
            OrderID order_id = (static_cast<OrderID>(agentID) << 8)| counter;
            counter++;
            auto order = std::make_unique<Order>(order_id, price, quantity, OrderType::Limit, side);
            auto command = std::make_unique<Command>(order);
            gateway.Push(std::move(command));
            //std::cout << "order_placed by " << agentID << " ";

            std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 100 + 50));
        }
    }
};

class AgentManager {
private:
    std::vector<std::unique_ptr<Agent>> agents;
public:
    AgentManager(int numberOfAgents, Gateway& gateway, std::atomic<bool>& flag){
        for(int i = 0; i<numberOfAgents; i++){
            agents.emplace_back(std::make_unique<Agent>(i+1, gateway, flag));
        }
    }
    
    void StartAll(){
        for(auto& agent: agents){
            agent->start();
        }
    }
    
    void join_all() {
        for (auto& agent : agents) {
            agent->join();
        }
    }
};
