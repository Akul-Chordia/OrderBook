//
//  agentmanager.h
//  OrderBook
//
class AgentManager {
private:
    std::vector<std::unique_ptr<Agent>> agents;
public:
    AgentManager(int numberOfAgents, Gateway& gateway, const OrderBook& orderbook, const Trades& trades, std::atomic<bool>& flag){
        for(int i = 0; i<numberOfAgents; i++){
            agents.emplace_back(std::make_unique<RetailAgent>(i+1, gateway, orderbook, trades, flag));
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
