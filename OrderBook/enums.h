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

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderID = std::uint64_t;
using TimeStamp = std::chrono::nanoseconds;

constexpr std::int32_t INVALID = std::numeric_limits<std::int32_t>::min();
