//
//  agentmanager.h
//  OrderBook
//
class AgentManager {
private:
    std::vector<std::unique_ptr<Agent>> agents;
public:
    AgentManager(const int* numberOfAgents, Gateway& gateway, const OrderBook& orderBook, const Trades& trades, std::atomic<bool>& flag){
        int agentID = 0;
        for(int i = 0; i<numberOfAgents[0]; i++){
            agents.emplace_back(std::make_unique<RetailAgent>(++agentID, gateway, orderBook, trades, flag));
        }
        for(int i = 0; i<numberOfAgents[1]; i++){
            agents.emplace_back(std::make_unique<HFTAgent>(++agentID, gateway, orderBook, trades, flag));
        }
        for(int i = 0; i<numberOfAgents[2]; i++){
            agents.emplace_back(std::make_unique<TWAPAgent>(++agentID, gateway, orderBook, trades, flag));
        }
        for(int i = 0; i<numberOfAgents[3]; i++){
            agents.emplace_back(std::make_unique<MarketMakerAgent>(++agentID, gateway, orderBook, trades, flag));
        }
        for(int i = 0; i<numberOfAgents[4]; i++){
            agents.emplace_back(std::make_unique<VWAPAgent>(++agentID, gateway, orderBook, trades, flag));
        }
        for(int i = 0; i<numberOfAgents[5]; i++){
            agents.emplace_back(std::make_unique<MomentumAgent>(++agentID, gateway, orderBook, trades, flag));
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
