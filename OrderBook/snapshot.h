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
    std::atomic<int> bufferIndex;

public:
    DataBuffer()
    : bufferIndex{0}
    {}

    void Write(const OrderBook& orderBook){
        int writeIndex = 1 - bufferIndex.load(std::memory_order_relaxed);
        
        Snapshot& snapshot = dataBuffers[writeIndex];
        snapshot.clear();

        for (const auto& [price, level] : orderBook.GetAsks()) {
            snapshot.asks.emplace_back(price, level.GetQuantity());
        }
        for (const auto& [price, level] : orderBook.GetBids()) {
            snapshot.bids.emplace_back(price, level.GetQuantity());
        }
        
        bufferIndex.store(writeIndex, std::memory_order_release);
    }

    const Snapshot Read(){
        int currentIndex = bufferIndex.load(std::memory_order_acquire);
        return dataBuffers[currentIndex];
    }
};
