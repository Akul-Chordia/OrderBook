#include "includes.h"
#include "enums.h"
#include "order.h"
#include "ordermanager.h"
#include "pricelevels.h"
#include "orderbook.h"
#include "gateway.h"
#include "trades.h"
#include "exchange.h"
#include "agents.h"
#include "agentmanager.h"
#include "testdummyorders.h"

void RunSimulation(Exchange& exchange, Gateway& gateway, AgentManager& agentManager, std::atomic<bool>& flag){
    CommandPtr command;
    int i = 0;
    while (flag || !gateway.IsEmpty()){
        gateway.WaitAndPop(command);
        switch (command->type){
            case CommandType::PlaceOrder:
                if (std::get<OrderPtr>(command->payload) == nullptr){
                    throw std::logic_error("the order is a nullptr?");
                }
                //                std::cout << "\norder added : " << std::get<OrderPtr>(command->payload)->GetOrderID() << " " << std::get<OrderPtr>(command->payload)->GetPrice() << " " <<
                //                static_cast<int>(std::get<OrderPtr>(command->payload)->GetSide()) << " " <<
                //                    std::get<OrderPtr>(command->payload)->GetQuantity() ;
                exchange.AddOrder(std::move(std::get<OrderPtr>(command->payload)));
                break;
            case CommandType::ModifyOrder:{
                ModifyOrderPayload& payload = std::get<ModifyOrderPayload>(command->payload);
                exchange.ModifyOrder(payload.orderID, payload.newPrice, payload.newQuantity);
                break;}
            case CommandType::CancelOrder:
                exchange.CancelOrder(std::get<OrderID>(command->payload));
                break;
        }
        
        i++;
        if (i==10000){
            std::cout << "\nTurning off agents\n";
            flag = false;
            agentManager.join_all();
        }
    }
};


int main(int argc, const char * argv[]) {
    Gateway gateway;
    OrderManager orderManager;
    OrderBook orderBook;
    Trades trades;
    Exchange exchange(orderBook, orderManager, trades);
    std::atomic<bool> flag(true);
    
    const int numberOfAgents[5] = {100,5,1,0,0};// {Retail, HFT, TWAP(buy-pressure), unimplemented agents}
    AgentManager agentManager(numberOfAgents, gateway, orderBook, trades, flag);
    
    std::cout << "Starting " << numberOfAgents << " concurrent agents..." << std::endl;
    agentManager.StartAll();

    std::thread simulationThread(RunSimulation, std::ref(exchange), std::ref(gateway), std::ref(agentManager), std::ref(flag));
    
    simulationThread.join();

    std::cout << "\033[2J\033[H" << std::flush;
    orderBook.PrintOrderBook();

    return 0;
}


