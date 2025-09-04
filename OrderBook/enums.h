//
//  enums.h
//  OrderBook
//
enum class OrderType{
    Market,
    Limit,
    FillOrKill,
    ImmediateOrCancel,
    INVALID
};

enum class OrderStatus{
    New,
    PartiallyFilled ,
    Filled,
    Cancelled,
    Rejected
};

enum class Side{
    Buy,
    Sell,
    INVALID
};

enum class AgentType{
    Retail,
    HFT
};

enum class CommandType{
    PlaceOrder,
    //ModifyOrder,
    CancelOrder
};

using Price = std::int32_t;  // 1 -> $0.01    (10132 -> $101.32)
using Quantity = std::uint32_t;
using OrderID = std::uint64_t;
using TimeStamp = std::chrono::nanoseconds;
using SpotPrice = std::int32_t;
SpotPrice spotPrice= 100;
constexpr std::int32_t INVALID = std::numeric_limits<std::int32_t>::min();

