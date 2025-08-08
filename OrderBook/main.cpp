#include "includes.h"
#include "enums.h"
#include "order.h"
#include "ordermanager.h"
#include "pricelevels.h"
#include "orderbook.h"
#include "exchange.h"
#include "agents.h"
#include "hft.h"


void dummy_start_orders(Exchange* exchange){
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    std::default_random_engine rng(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> qty_dist(1, 20);
    std::uniform_real_distribution<double> price_dist1(90.0, 99.0);
    std::uniform_real_distribution<double> price_dist2(101.0, 110.0);

    for (int i = 1; i <= 100; i+=2) {
        int id = i;
        double price = std::round(price_dist1(rng) * 100.0) / 100.0;
        int qty = qty_dist(rng);
        Side side = Side::Buy;
        TimeStamp ts = now + std::chrono::milliseconds(i);

        auto order = std::make_unique<Order>(id, price, qty, OrderType::Limit, side, ts);
        exchange->dummyAddOrder(std::move(order));
        std::cout << "\033[2J\033[H" << std::flush;
        exchange->printbook();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        id++;
        price = std::round(price_dist2(rng) * 100.0) / 100.0;
        qty = qty_dist(rng);
        side = Side::Sell;
        ts = now + std::chrono::milliseconds(i);

        order = std::make_unique<Order>(id, price, qty, OrderType::Limit, side, ts);
        exchange->dummyAddOrder(std::move(order));
        std::cout << "\033[2J\033[H" << std::flush;
        exchange->printbook();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, const char * argv[]) {
    OrderManager orderManager;
    OrderBook orderBook;
    Exchange exchange(orderBook, orderManager);
    
    Exchange* exchangeptr = &exchange;

    dummy_start_orders(exchangeptr);

    orderBook.PrintOrderBook();

    return 0;

}


