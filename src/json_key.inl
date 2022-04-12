namespace cedar {

template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline json_key::json_key(Int i) : t_(INTEGER), i_(i) {}

inline json_key::json_key(const char *s) : t_(STRING) { new (&s_) std::string(s); }
inline json_key::json_key(const std::string &s) : t_(STRING) { new (&s_) std::string(s); }
inline json_key::json_key(std::string &&s) : t_(STRING) { new (&s_) std::string(std::move(s)); }

inline json_key::json_key(const json_key &other) : t_(NONE) { *this = other; }
inline json_key::json_key(json_key &&other) : t_(NONE) { *this = std::move(other); }

inline json_key::~json_key() { destructor(); }

inline json_key &json_key::operator=(const json_key &other) {
    change_type(other.t_);
    switch (t_) {
        case INTEGER:
            i_ = other.i_;
            break;

        case STRING:
            s_ = other.s_;
            break;

        default:;
    }
    return *this;
}
inline json_key &json_key::operator=(json_key &&other) {
    change_type(other.t_);
    switch (t_) {
        case INTEGER:
            i_ = other.i_;
            break;

        case STRING:
            s_ = std::move(other.s_);
            break;

        default:;
    }
    other.change_type(NONE);
    return *this;
}

inline bool json_key::operator<(const json_key &other) const {
    if (t_ != other.t_) {
        return t_ < other.t_;
    }
    switch (t_) {
        case INTEGER:
            return i_ < other.i_;

        case STRING:
            return s_ < other.s_;

        default:;
    }
    return false;
}

inline json_key::operator std::string() const {
    if (t_ == NONE) {
        throw json_exception("无法将 Null 状态下的 json_key 转化为 std::string.");
    }
    std::string s;
    s += t_ == STRING ? "\"" : "";
    s += t_ == STRING ? s_ : std::to_string(i_);
    s += t_ == STRING ? "\"" : "";
    return s;
}

inline void json_key::constructor() {
    if (t_ == STRING) {
        new (&s_) std::string();
    }
}

inline void json_key::destructor() {
    if (t_ == STRING) {
        s_.~basic_string();
    }
}

inline void json_key::change_type(decltype(t_) t) {
    if (t == t_) {
        return;
    }
    destructor();
    t_ = t;
    constructor();
}

inline std::ostream &operator<<(std::ostream &out, const json_key &j) {
    out << static_cast<std::string>(j);
    return out;
}

}