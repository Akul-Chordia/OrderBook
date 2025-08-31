#include "includes.h"
#include "enums.h"
#include "order.h"
#include "ordermanager.h"
#include "pricelevels.h"
#include "orderbook.h"
#include "gateway.h"
#include "exchange.h"
#include "agents.h"
#include "hft.h"


void dummy_start_orders(Exchange* exchange, Gateway* gateway){
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    std::default_random_engine rng(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> qty_dist(1, 20);
    std::uniform_real_distribution<double> price_dist1(90.0, 110.0);
    //std::uniform_real_distribution<double> price_dist2(95.0, 109.0);
    int buys = 0;
    int sells = 0;

    for (int id = 1; id <= 10000; id++) {
        double price = std::round(price_dist1(rng) * 100.0);
        int qty = qty_dist(rng);
        Side side = Side::Buy;
        TimeStamp ts = now;

        auto order = std::make_unique<Order>(id, price, qty, OrderType::Limit, side, ts);
        auto command = std::make_unique<Command>(order);
        //exchange->AddOrder(std::move(order));
        gateway->Push(std::move(command));
        buys++;
        //std::cout << "\033[2J\033[H" << std::flush;
        //exchange->PrintBook();
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        if ((qty)%2==0){
//            continue;
//        }
        id++;
        price = std::round(price_dist1(rng) * 100.0);
        qty = qty_dist(rng);
        side = Side::Sell;
        ts = now;
        order = std::make_unique<Order>(id, price, qty, OrderType::Limit, side, ts);
        command = std::make_unique<Command>(order);
        //exchange->AddOrder(std::move(order));
        gateway->Push(std::move(command));
        sells++;
        //std::cout << "\033[2J\033[H" << std::flush;
        //exchange->PrintBook();
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << " buys = "<< buys << "\nsells = " << sells;
}

int main(int argc, const char * argv[]) {
    Gateway gateway;
    OrderManager orderManager;
    OrderBook orderBook;
    Exchange exchange(orderBook, orderManager);
    std::thread agents_thread(dummy_start_orders, &exchange, &gateway);

    //dummy_start_orders(exchangeptr, gatewayptr);
    CommandPtr command;
    while (agents_thread.joinable()){
        
        
        while (gateway.Pop(command)){
            switch (command->type){
                case CommandType::PlaceOrder:
                    exchange.AddOrder(std::move(std::get<OrderPtr>(command->payload)));
                    break;
//                case CommandType::ModifyOrder:
//                    orderManager.ModifyOrder(std::move(std::get<OrderPtr>(command->payload)));
//                    Not implemented yet, skip this type
//                    break;
                case CommandType::CancelOrder:
                    exchange.CancelOrder(std::get<OrderID>(command->payload));
                    break;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }

    orderBook.PrintOrderBook();

    return 0;

}


