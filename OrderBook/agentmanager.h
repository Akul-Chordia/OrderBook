//
//  agentmanager.h
//  OrderBook
//

class RandomAgent; class HFTAgent; class VWAPAgent; class RetailAgent;

class Agent {
protected:
    int agentID;
    int counter = 0;
    Gateway& gateway;
    const OrderBook& orderbook;
    const Trades& trades;
    std::thread agent_thread;
    std::atomic<bool>& flag;
    std::mt19937 rng;
public:
    Agent(int agentID, Gateway& gateway, OrderBook& orderbook, Trades& trades, std::atomic<bool>& flag)
    :agentID{agentID},
    gateway{gateway},
    orderbook{orderbook},
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
        return ((static_cast<OrderID>(agentID) << 56) | counter);
    }
};


class AgentManager {
private:
    std::vector<std::unique_ptr<Agent>> agents;
public:
    AgentManager(int numberOfAgents, Gateway& gateway, OrderBook& orderbook, Trades& trades, std::atomic<bool>& flag){
        for(int i = 0; i<numberOfAgents; i++){
            agents.emplace_back(std::make_unique<RandomAgent>(i+1, gateway, orderbook, trades, flag));
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
