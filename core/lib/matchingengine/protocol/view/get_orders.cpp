#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/protocol/view/get_orders.h>
#include <sstream>
#include <utility>

namespace core::protocol::view
{

    Order::Order(
        const OrderId& id, const Price& price, const Quantity& quantity,
        const Side& side, const Timestamp& timestamp
    )
        : id(id), price(price), quantity(quantity), side(side),
          timestamp(timestamp)
    {
    }

    std::ostream& Order::operator<<(std::ostream& os) const
    {
        os << "Order{";
        os << "id: " << id << ", ";
        os << "price: " << price << ", ";
        os << "quantity: " << quantity << ", ";
        os << "side: " << side << ", ";
        os << "timestamp: " << timestamp;
        os << "}";
        return os;
    }

    GetOrdersRequest::GetOrdersRequest(std::string username)
        : f_username(std::move(username))
    {
    }

    MessageType GetOrdersRequest::type()
    {
        return MessageType::GetOrdersRequest;
    }

    void GetOrdersRequest::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_username, dst, &offset);
    }

    void GetOrdersRequest::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_username = deserialize_string(src, &offset);
    }

    void GetOrdersRequest::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string GetOrdersRequest::to_string() const
    {
        std::ostringstream os;
        os << "GetOrdersRequest{";
        os << "username: " << f_username;
        os << "}";
        return os.str();
    }

    size_t GetOrdersRequest::size() const
    {
        return sizeof(StrLen) + f_username.length();
    }

    std::string GetOrdersRequest::username() const
    {
        return f_username;
    }

    GetOrdersResponse::GetOrdersResponse(
        std::unordered_map<Symbol, std::vector<Order>> orders_by_symbol
    )
        : f_orders_by_symbol(std::move(orders_by_symbol))
    {
    }

    GetOrdersResponse::GetOrdersResponse(
        std::unordered_map<Symbol, std::initializer_list<Order>>
            orders_by_symbol
    )
    {
        f_orders_by_symbol.reserve(orders_by_symbol.size());
        for (auto&& [symbol, orders] : orders_by_symbol) {
            f_orders_by_symbol[symbol] = std::vector<Order>{orders};
        }
    }

    MessageType GetOrdersResponse::type()
    {
        return MessageType::GetOrdersResponse;
    }

    void GetOrdersResponse::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_uint8(
            static_cast<uint8_t>(f_orders_by_symbol.size()), dst, &offset
        );
        for (const auto& [symbol, orders] : f_orders_by_symbol) {
            serialize_string(symbol, dst, &offset);
            serialize_uint8(static_cast<uint8_t>(orders.size()), dst, &offset);
            for (const auto& order : orders) {
                serialize_simple_struct(order, dst, &offset);
            }
        }
    }

    void GetOrdersResponse::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_orders_by_symbol.clear();

        const size_t n_items = deserialize_uint8(src, &offset);
        for (size_t i = 0; i < n_items; ++i) {
            auto symbol = deserialize_string(src, &offset);
            const size_t n_orders = deserialize_uint8(src, &offset);
            std::vector<Order> orders(n_orders);
            for (size_t j = 0; j < n_orders; ++j) {
                orders[j] = deserialize_simple_struct<Order>(src, &offset);
            }
            f_orders_by_symbol[std::move(symbol)] = std::move(orders);
        }
    }

    void GetOrdersResponse::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string GetOrdersResponse::to_string() const
    {
        std::ostringstream os;
        os << "GetOrdersResponse{";
        for (const auto& [symbol, orders] : f_orders_by_symbol) {
            os << "{" << symbol << ": [";
            for (const auto& order : orders) {
                order.operator<<(os) << ",";
            }
            os << "]},";
        }
        os << "}";
        return os.str();
    }

    size_t GetOrdersResponse::size() const
    {
        size_t size = 0;
        for (const auto& [symbol, orders] : f_orders_by_symbol) {
            size += sizeof(StrLen) + symbol.length();
            size += sizeof(Order) + orders.size();
        }
        return size;
    }

    std::unordered_map<Symbol, std::vector<Order>>
    GetOrdersResponse::orders() const
    {
        return f_orders_by_symbol;
    }
} // namespace core::protocol::view
