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
#include "snapshot.h"
#include "gui.h"
#include "windows.h"
//#include "testdummyorders.h"

void RunSimulation(Exchange& exchange, Gateway& gateway, AgentManager& agentManager, DataBuffer& dataBuffer, std::atomic<bool>& flag){
    CommandPtr command;
    auto lastUpdateTime = std::chrono::steady_clock::now();
    const auto updateInterval = std::chrono::milliseconds(16);
    int i = 0;
    while (flag || !gateway.IsEmpty()){
        if(gateway.WaitAndPop(command)){
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
        }
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - lastUpdateTime > updateInterval) {
            dataBuffer.Write(exchange.GetOrderBook(), exchange.GetTrades());
            lastUpdateTime = currentTime;
        }
        
        i++;
        if (i==500000){
            std::cout << "\nTurning off agents\n";
            flag = false;
            agentManager.join_all();
        }
        
//        if (i%5000==0){
//            std::cout << "\033[2J\033[H" << std::flush;
//            exchange.GetOrderBook().PrintOrderBook();
//        }
    }
};


int main(int argc, const char * argv[]) {
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    Gateway gateway;
    OrderManager orderManager;
    OrderBook orderBook;
    Trades trades;
    Exchange exchange(orderBook, orderManager, trades);
    DataBuffer dataBuffer;
    std::atomic<bool> flag(true);
    
    const int numberOfAgents[5] = {100,5,1,0,0};// {Retail, HFT, TWAP(sell-pressure), unimplemented agents}
    AgentManager agentManager(numberOfAgents, gateway, orderBook, trades, flag);
    
    std::cout << "Starting " << numberOfAgents << " concurrent agents..." << std::endl;
    agentManager.StartAll();

    std::thread simulationThread(RunSimulation, std::ref(exchange), std::ref(gateway), std::ref(agentManager), std::ref(dataBuffer), std::ref(flag));
    
    GUI gui(std::ref(dataBuffer), std::ref(gateway), std::ref(flag));
    gui.run();

    simulationThread.join();

    std::cout << "\033[2J\033[H" << std::flush;
    orderBook.PrintOrderBook();

    return 0;
}


