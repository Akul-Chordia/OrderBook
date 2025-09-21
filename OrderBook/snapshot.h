//
//  snapshot.h
//  OrderBook
//
struct Snapshot {
    std::vector<std::pair<Price, Quantity>> asks;
    std::vector<std::pair<Price, Quantity>> bids;

    Snapshot() {
        asks.reserve(1000);
        bids.reserve(1000);
    }

    void clear() {
        asks.clear();
        bids.clear();
    }
};

class DataBuffer {
private:
    Snapshot dataBuffers[2];
    std::atomic<int> BufferIndex;

public:
    DataBuffer()
    : BufferIndex{0}
    {}

    void Write(const OrderBook& orderBook){
        int currentIndex = BufferIndex.load(std::memory_order_relaxed);
        Snapshot& snapshot = dataBuffers[currentIndex];
        
        snapshot.clear();

        for (const auto& [price, level] : orderBook.GetAsks()) {
            snapshot.asks.emplace_back(price, level.GetQuantity());
        }
        for (const auto& [price, level] : orderBook.GetBids()) {
            snapshot.bids.emplace_back(price, level.GetQuantity());
        }
    }

    const Snapshot Read(){
        int currentIndex = BufferIndex.load(std::memory_order_relaxed);
        int nextIndex = 1 - currentIndex;
        BufferIndex.store(nextIndex, std::memory_order_relaxed);
        return dataBuffers[currentIndex];
    }
};
