
#include "json.hpp"

namespace cedar {

inline json::json() : t_(type::null) {}

template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline json::json(Int i) : t_(type::integer), i_(i) {}

inline json::json(bool i) : t_(type::boolean), i_(i) {}

inline json::json(float d) : t_(type::decimal), d_(d) {}
inline json::json(double d) : t_(type::decimal), d_(d) {}

inline json::json(const char *s) : t_(type::string) { new (&s_) string(s); }
inline json::json(const std::string &s) : t_(type::string) { new (&s_) string(s); }
inline json::json(std::string &&s) : t_(type::string) { new (&s_) string(std::move(s)); }

inline json::json(const null &) : t_(type::null) {}
inline json::json(const array &a) : t_(type::array) {
    constructor();
    for (auto &i : a.data()) {
        push_back(i);
    }
}
inline json::json(const object &o) : t_(type::object) {
    constructor();
    for (auto &i : o.data()) {
        operator[](i.first) = i.second;
    }
}

inline json::json(const json &other) : t_(type::null) { *this = other; }
inline json::json(json &&other) noexcept : t_(type::null) { *this = std::move(other); }

inline json::~json() { destructor(); }

inline json &json::operator=(const json &other) {
    change_type(other.t_);
    switch (t_) {
        case type::boolean:
        case type::integer:
            i_ = other.i_;
            break;

        case type::decimal:
            d_ = other.d_;
            break;

        case type::string:
            s_ = other.s_;
            break;

        case type::array:
        case type::object:
            m_ = other.m_;
            break;

        default:;
    }
    return *this;
}
inline json &json::operator=(json &&other) noexcept {
    change_type(other.t_);
    switch (t_) {
        case type::boolean:
        case type::integer:
            i_ = other.i_;
            break;

        case type::decimal:
            d_ = other.d_;
            break;

        case type::string:
            s_ = std::move(other.s_);
            break;

        case type::array:
        case type::object:
            m_ = std::move(other.m_);
            break;

        default:;
    }
    other.change_type(type::null);
    return *this;
}

inline json &json::operator[](const json_key &key) {
    if (is_null()) {
        t_ = key.is_integer() ? type::array : type::object;
        constructor();
    }
    switch (t_) {
        case type::array:
            if (!key.is_integer()) {
                throw json_exception("Array 状态下只能使用整数型 json_key.");
            }
            return m_[key];
        case type::object:
            if (!key.is_string()) {
                throw json_exception("Object 状态下只能使用字符串型 json_key.");
            }
            return m_[key];

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 operator[].");
}

inline json &json::at(const json_key &key) {
    switch (t_) {
        case type::array:
            if (!key.is_integer()) {
                throw json_exception("Array 状态下只能使用整数型 json_key.");
            }
            return m_.at(key);
        case type::object:
            if (!key.is_string()) {
                throw json_exception("Object 状态下只能使用字符串型 json_key.");
            }
            return m_.at(key);

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 at().");
}
inline const json &json::at(const json_key &key) const {
    switch (t_) {
        case type::array:
            if (!key.is_integer()) {
                throw json_exception("Array 状态下只能使用整数型 json_key.");
            }
            return m_.at(key);
        case type::object:
            if (!key.is_string()) {
                throw json_exception("Object 状态下只能使用字符串型 json_key.");
            }
            return m_.at(key);

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 at().");
}

inline std::string json::dump() const {
    std::string s;
    switch (t_) {
        case type::null:
            s = "null";
            break;

        case type::boolean:
            s = i_ ? "true" : "false";
            break;

        case type::integer:
            s = std::to_string(i_);
            break;

        case type::decimal: {
            std::stringstream ss;
            ss << std::setprecision(std::numeric_limits<double>::digits10 + 1) << d_;
            ss >> s;
        } break;

        case type::string:
            s = "\"";
            for (size_t i = 0; i < s_.size(); ++i) {
                switch (s_[i]) {
                    case '\"':
                        if (!json_parser::is_escape_double_quotes(s_, i)) {
                            s += '\\';
                        }
                        s += '\"';
                        break;
                    case '\r':
                        s += "\\r";
                        break;
                    case '\n':
                        s += "\\n";
                        break;
                    default:
                        s += s_[i];
                }
            }
            s += "\"";
            break;

        case type::array: {
            s = "[";
            long long index = 0;
            for (auto iter = m_.begin(); iter != m_.end(); ++iter, ++index) {
                for (; index < iter->first.i_; ++index) {
                    if (index != 0) {
                        s += ",";
                    }
                    s += "null";
                }
                if (index != 0) {
                    s += ",";
                }
                s += iter->second.dump();
            }
            s += "]";
        } break;

        case type::object:
            s = "{";
            for (auto iter = m_.begin(); iter != m_.end(); ++iter) {
                if (iter != m_.begin()) {
                    s += ",";
                }
                s += iter->first.dump();
                s += ":";
                s += iter->second.dump();
            }
            s += "}";
            break;
    }
    return s;
}

template <>
inline json::integer &json::cast<json::integer &>() {
    if (is_integer()) {
        return i_;
    }
    throw json_exception("非 Integer 状态无法转换为 integer &.");
}
template <>
inline json::decimal &json::cast<json::decimal &>() {
    if (is_decimal()) {
        return d_;
    }
    throw json_exception("非 Decimal 状态无法转换为 decimal &.");
}
template <>
inline json::string &json::cast<json::string &>() {
    if (is_string()) {
        return s_;
    }
    throw json_exception("非 String 状态无法转换为 string &.");
}

template <class T>
inline T json::cast() const {
    return cast_int<T>();
}
template <>
inline bool json::cast<bool>() const {
    if (is_boolean()) {
        return i_;
    }
    throw json_exception("非 Bool 状态无法转换为 bool.");
}
template <>
inline float json::cast<float>() const {
    if (is_decimal()) {
        return static_cast<float>(d_);
    }
    throw json_exception("非 Decimal 状态无法转换为 float.");
}
template <>
inline double json::cast<double>() const {
    if (is_decimal()) {
        return d_;
    }
    throw json_exception("非 Decimal 状态无法转换为 double.");
}
template <>
inline std::string json::cast<std::string>() const {
    if (is_string()) {
        return s_;
    } else {
        return dump();
    }
}
template <>
inline const std::string &json::cast<const std::string &>() const {
    if (is_string()) {
        return s_;
    }
    throw json_exception("非 String 状态无法转换为 const string &.");
}

inline void json::push_back(const json &value) {
    if (is_null()) {
        t_ = type::array;
        constructor();
    }
    if (!is_array()) {
        throw json_exception("非 Array 状态无法使用 push_back.");
    }
    m_[len()] = value;
}
inline void json::push_back(json &&value) {
    if (is_null()) {
        t_ = type::array;
        constructor();
    }
    if (!is_array()) {
        throw json_exception("非 Array 状态无法使用 push_back.");
    }
    m_[len()] = std::move(value);
}

inline json &json::back() {
    if (!is_array()) {
        throw json_exception("非 Array 状态无法使用 back.");
    }
    return (*--m_.end()).second;
}
inline const json &json::back() const {
    if (!is_array()) {
        throw json_exception("非 Array 状态无法使用 back.");
    }
    return (*--m_.cend()).second;
}

inline void json::erase(const json_key &key) {
    switch (t_) {
        case type::array:
            if (!key.is_integer()) {
                throw json_exception(
                    "Array 状态下只能使用整数型 json_key.");
            }
        case type::object:
            m_.erase(key);
            return;

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 erase().");
}

inline size_t json::size() const {
    switch (t_) {
        case type::string:
            return s_.size();

        case type::array:
        case type::object:
            return m_.size();

        default:;
    }
    throw json_exception("非 String 或 Array 或 Object 状态无法使用 size().");
}
inline size_t json::len() const {
    switch (t_) {
        case type::string:
            return s_.size();

        case type::array:
            return m_.empty() ? 0 : (*--m_.cend()).first.i_ + 1;

        case type::object:
            return m_.size();

        default:;
    }
    throw json_exception("非 String 或 Array 或 Object 状态无法使用 len().");
}

inline void json::fill_array(const json &sam) {
    if (!is_array()) {
        throw json_exception("非 Array 状态无法使用 fill_array.");
    }
    long long index = 0;
    for (auto iter = m_.begin(); iter != m_.end(); ++iter, ++index) {
        for (; index < iter->first.i_; ++index) {
            operator[](index) = sam;
        }
    }
}

inline void json::clear() { change_type(type::null); }

#define CEDAR_JSON_ITERATOR_DEFINE_(iter_type, name, limit)                    \
    inline json::iter_type json::name() limit {                                \
        if (!is_array() && !is_object()) {                         \
            throw json_exception("非 Array 或 Object 状态无法调用 " #name "()."); \
        }                                                                      \
        return m_.name();                                                      \
    }

CEDAR_JSON_ITERATOR_(DEFINE);
#undef CEDAR_JSON_ITERATOR_DEFINE_

inline json json::parse(const std::string &json_str) {
    return json_parser(json_str).do_it();
}

inline json::type json::get_type() const { return t_; }
inline bool json::is_null() const { return t_ == type::null; }
inline bool json::is_boolean() const { return t_ == type::boolean; }
inline bool json::is_integer() const { return t_ == type::integer; }
inline bool json::is_decimal() const { return t_ == type::decimal; }
inline bool json::is_string() const { return t_ == type::string; }
inline bool json::is_array() const { return t_ == type::array; }
inline bool json::is_object() const { return t_ == type::object; }

inline void json::constructor() {
    switch (t_) {
        case type::string:
            new (&s_) std::string();
            return;

        case type::array:
        case type::object:
            new (&m_) std::map<json_key, json>();
            return;

        default:;
    }
}

inline void json::destructor() {
    switch (t_) {
        case type::string:
            s_.~basic_string();
            return;

        case type::array:
        case type::object:
            m_.~map();
            return;

        default:;
    }
}

inline void json::change_type(type t) {
    if (t == t_) {
        return;
    }
    destructor();
    t_ = t;
    constructor();
}

template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline Int json::cast_int() const {
    if (is_integer()) {
        return i_;
    }
    throw json_exception("非 Integer 状态无法转换.");
}

inline std::ostream &operator<<(std::ostream &out, const json &j) {
    out << j.dump();
    return out;
}

}  // namespace cedar
