
#include "json.hpp"

namespace cedar {

inline json_exception::json_exception(const char *info) : std::runtime_error(info) {}
inline json_exception::json_exception(const std::string &info) : std::runtime_error(info) {}

inline json_exception json_exception::cannot_call(const char *func, json::type type) {
    return json_exception(type_name(type) + " status cannot call '" + func + "'.");
}

inline json_exception json_exception::cannot_use_key(const char *func, json::type json_type, json_key::type key_type) {
    return json_exception(type_name(json_type)
                        + " status call '" + func + "' cannot use "
                        + type_name(static_cast<json::type>(key_type))
                        + " key.");
}

inline std::string json_exception::type_name(json::type type) {
    switch (type) {
        case json::type::null:
            return "Null";
        case json::type::boolean:
            return "Boolean";
        case json::type::integer:
            return "Integer";
        case json::type::decimal:
            return "Decimal";
        case json::type::string:
            return "String";
        case json::type::array:
            return "Array";
        case json::type::object:
            return "Object";
    }
    return "!!!type_name error!!!";
}

}  // namespace cedar
