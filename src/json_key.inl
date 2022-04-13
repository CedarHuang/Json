namespace cedar {

template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline json_key::json_key(Int i) : t_(type::integer), i_(i) {}

inline json_key::json_key(const char *s) : t_(type::string) { new (&s_) std::string(s); }
inline json_key::json_key(const std::string &s) : t_(type::string) { new (&s_) std::string(s); }
inline json_key::json_key(std::string &&s) : t_(type::string) { new (&s_) std::string(std::move(s)); }

inline json_key::json_key(const json_key &other) : t_(type::null) { *this = other; }
inline json_key::json_key(json_key &&other) noexcept : t_(type::null) { *this = std::move(other); }

inline json_key::~json_key() { destructor(); }

inline json_key &json_key::operator=(const json_key &other) {
    change_type(other.t_);
    switch (t_) {
        case type::integer:
            i_ = other.i_;
            break;

        case type::string:
            s_ = other.s_;
            break;

        default:;
    }
    return *this;
}
inline json_key &json_key::operator=(json_key &&other) noexcept {
    change_type(other.t_);
    switch (t_) {
        case type::integer:
            i_ = other.i_;
            break;

        case type::string:
            s_ = std::move(other.s_);
            break;

        default:;
    }
    other.change_type(type::null);
    return *this;
}

inline bool json_key::operator<(const json_key &other) const {
    if (t_ != other.t_) {
        return t_ < other.t_;
    }
    switch (t_) {
        case type::integer:
            return i_ < other.i_;

        case type::string:
            return s_ < other.s_;

        default:;
    }
    return false;
}

inline std::string json_key::dump() const {
    if (is_null()) {
        return "";
    }
    std::string s;
    s += is_string() ? "\"" : "";
    s += is_string() ? s_ : std::to_string(i_);
    s += is_string() ? "\"" : "";
    return s;
}

template <>
inline json_key::integer &json_key::cast<json_key::integer &>() {
    if (is_integer()) {
        return i_;
    }
    throw json_exception("非 Integer 状态无法转换为 integer &.");
}
template <>
inline json_key::string &json_key::cast<json_key::string &>() {
    if (is_string()) {
        return s_;
    }
    throw json_exception("非 String 状态无法转换为 string &.");
}

template <class T>
inline T json_key::cast() const {
    return cast_int<T>();
}
template <>
inline std::string json_key::cast<std::string>() const {
    if (is_string()) {
        return s_;
    } else {
        return dump();
    }
}
template <>
inline const std::string &json_key::cast<const std::string &>() const {
    if (is_string()) {
        return s_;
    }
    throw json_exception("非 String 状态无法转换为 const string &.");
}

inline json_key::type json_key::get_type() const { return t_; }
inline bool json_key::is_null() const { return t_ == type::null; }
inline bool json_key::is_integer() const { return t_ == type::integer; }
inline bool json_key::is_string() const { return t_ == type::string; }

inline void json_key::constructor() {
    if (is_string()) {
        new (&s_) std::string();
    }
}

inline void json_key::destructor() {
    if (is_string()) {
        s_.~basic_string();
    }
}

inline void json_key::change_type(type t) {
    if (t == t_) {
        return;
    }
    destructor();
    t_ = t;
    constructor();
}

template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline Int json_key::cast_int() const {
    if (is_integer()) {
        return i_;
    }
    throw json_exception("非 Integer 状态无法转换.");
}

inline std::ostream &operator<<(std::ostream &out, const json_key &j) {
    out << j.cast();
    return out;
}

}