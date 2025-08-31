//
//  testdummyorders.h
//  OrderBook
//
void dummy_start_orders(Gateway* gateway){
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
        std::cout<< "command pushed";
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
