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
    ModifyOrder,
    CancelOrder
};


using Price = std::int64_t;  // 1 -> $0.01    (10132 -> $101.32)
using Quantity = std::int64_t;
using OrderID = std::uint64_t;
using TimeStamp = std::chrono::nanoseconds;

using SpotPrice = std::int32_t;
constexpr std::int32_t INVALID = std::numeric_limits<std::int32_t>::min();


// ImGui Helper
// Template helper to allow ImGui::RadioButton to work directly with 'enum class' types
namespace ImGui{
    template <typename T>
    typename std::enable_if<std::is_enum<T>::value, bool>::type
    RadioButton(const char* label, T* v, T v_button){
        return RadioButton(label, reinterpret_cast<int*>(v), static_cast<int>(v_button));
    }
}
