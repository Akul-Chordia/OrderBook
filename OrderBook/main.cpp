#include "includes.h"
#include "enums.h"
#include "order.h"
#include "ordermanager.h"
#include "pricelevels.h"
#include "orderbook.h"
#include "gateway.h"
#include "trades.h"
#include "exchange.h"
#include "agentmanager.h"
#include "agents.h"
#include "hft.h"
#include "testdummyorders.h"


int main(int argc, const char * argv[]) {
    Gateway gateway;
    OrderManager orderManager;
    OrderBook orderBook;
    Trades trades;
    Exchange exchange(orderBook, orderManager, trades);
    std::atomic<bool> flag(true);
    const int numberOfAgents = 200;
    AgentManager agentManager(numberOfAgents, gateway, orderBook, trades, flag);
    std::cout << "Starting " << numberOfAgents << " concurrent agents..." << std::endl;
    agentManager.StartAll();
    
    
    //std::thread agents_thread(dummy_start_orders, &exchange, &gateway);
    //dummy_start_orders(exchangeptr, gatewayptr);
    
    CommandPtr command;
    int i = 0;
    while (i<10000){
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
//            case CommandType::ModifyOrder:
//                orderManager.ModifyOrder(std::move(std::get<OrderPtr>(command->payload)));
//                Not implemented yet, skip this type
//                break;
            case CommandType::CancelOrder:
                exchange.CancelOrder(std::get<OrderID>(command->payload));
                break;
        }
        
        if (i%1000==0){
            std::cout << "\033[2J\033[H" << std::flush;
            orderBook.PrintOrderBook();
        }
        i++;
    }
    
    std::cout << "\nTurning off agents\n";
    
    flag = false;
    agentManager.join_all();
    
    while (!gateway.IsEmpty()) {
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
//            case CommandType::ModifyOrder:
//                orderManager.ModifyOrder(std::move(std::get<OrderPtr>(command->payload)));
//                Not implemented yet, skip this type
//                break;
            case CommandType::CancelOrder:
                exchange.CancelOrder(std::get<OrderID>(command->payload));
                break;
        }
    }
//    std::cout << "\033[2J\033[H" << std::flush;
//    orderBook.PrintOrderBook();
//    
//    OrderPtr neworder = std::make_unique<Order>(999299292922, 100, OrderType::Market, Side::Buy);
//    CommandPtr cmd = std::make_unique<Command>(neworder);
//    exchange.AddOrder(std::move(std::get<OrderPtr>(cmd->payload)));
    
    std::cout << "\033[2J\033[H" << std::flush;
    orderBook.PrintOrderBook();
//
//    return 0;

}


