namespace cedar {

inline json::json() : t_(NONE) {}

template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline json::json(Int i) : t_(INTEGRAL), i_(i) {}

inline json::json(bool i) : t_(BOOL), i_(i) {}

inline json::json(float d) : t_(DECIMAL), d_(d) {}
inline json::json(double d) : t_(DECIMAL), d_(d) {}

inline json::json(const char *s) : t_(STRING) { new (&s_) std::string(s); }
inline json::json(const std::string &s) : t_(STRING) { new (&s_) std::string(s); }
inline json::json(std::string &&s) : t_(STRING) { new (&s_) std::string(std::move(s)); }

inline json::json(const null &) : t_(NONE) {}
inline json::json(const array &a) : t_(ARRAY) {
    constructor();
    for (auto &i : a.data()) {
        push_back(i);
    }
}
inline json::json(const object &o) : t_(OBJECT) {
    constructor();
    for (auto &i : o.data()) {
        operator[](i.first) = i.second;
    }
}

inline json::json(const json &other) : t_(NONE) { *this = other; }
inline json::json(json &&other) : t_(NONE) { *this = std::move(other); }

inline json::~json() { destructor(); }

inline json &json::operator=(const json &other) {
    change_type(other.t_);
    switch (t_) {
        case BOOL:
        case INTEGRAL:
            i_ = other.i_;
            break;

        case DECIMAL:
            d_ = other.d_;
            break;

        case STRING:
            s_ = other.s_;
            break;

        case ARRAY:
        case OBJECT:
            m_ = other.m_;
            break;

        default:;
    }
    return *this;
}
inline json &json::operator=(json &&other) {
    change_type(other.t_);
    switch (t_) {
        case BOOL:
        case INTEGRAL:
            i_ = other.i_;
            break;

        case DECIMAL:
            d_ = other.d_;
            break;

        case STRING:
            s_ = std::move(other.s_);
            break;

        case ARRAY:
        case OBJECT:
            m_ = std::move(other.m_);
            break;

        default:;
    }
    other.change_type(NONE);
    return *this;
}

inline json &json::operator[](const json_key &key) {
    if (t_ == NONE) {
        t_ = key.t_ == key.INTEGRAL ? ARRAY : OBJECT;
        constructor();
    }
    switch (t_) {
        case ARRAY:
            if (key.t_ != key.INTEGRAL) {
                throw json_exception("Array 状态下只能使用整数型 json_key.");
            }
            return m_[key];
        case OBJECT:
            if (key.t_ != key.STRING) {
                throw json_exception("Object 状态下只能使用字符串型 json_key.");
            }
            return m_[key];

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 operator[].");
}

inline json &json::at(const json_key &key) {
    switch (t_) {
        case ARRAY:
            if (key.t_ != key.INTEGRAL) {
                throw json_exception("Array 状态下只能使用整数型 json_key.");
            }
            return m_.at(key);
        case OBJECT:
            if (key.t_ != key.STRING) {
                throw json_exception("Object 状态下只能使用字符串型 json_key.");
            }
            return m_.at(key);

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 at().");
}
inline const json &json::at(const json_key &key) const {
    switch (t_) {
        case ARRAY:
            if (key.t_ != key.INTEGRAL) {
                throw json_exception("Array 状态下只能使用整数型 json_key.");
            }
            return m_.at(key);
        case OBJECT:
            if (key.t_ != key.STRING) {
                throw json_exception("Object 状态下只能使用字符串型 json_key.");
            }
            return m_.at(key);

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 at().");
}

inline json::operator std::string() const {
    std::string s;
    switch (t_) {
        case NONE:
            s = "null";
            break;

        case BOOL:
            s = i_ ? "true" : "false";
            break;

        case INTEGRAL:
            s = std::to_string(i_);
            break;

        case DECIMAL: {
            std::stringstream ss;
            ss << std::setprecision(std::numeric_limits<double>::digits10 + 1) << d_;
            ss >> s;
        } break;

        case STRING:
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

        case ARRAY: {
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
                s += iter->second;
            }
            s += "]";
        } break;

        case OBJECT:
            s = "{";
            for (auto iter = m_.begin(); iter != m_.end(); ++iter) {
                if (iter != m_.begin()) {
                    s += ",";
                }
                s += iter->first;
                s += ":";
                s += iter->second;
            }
            s += "}";
            break;
    }
    return s;
}

template <class T>
inline T json::cast() {
    return cast_int<T>();
}
template <>
inline bool json::cast<bool>() {
    if (t_ == BOOL) {
        return i_;
    }
    throw json_exception("非 Bool 状态无法转换为 bool.");
}
template <>
inline float json::cast<float>() {
    if (t_ == DECIMAL) {
        return d_;
    }
    throw json_exception("非 Decimal 状态无法转换为 float.");
}
template <>
inline double json::cast<double>() {
    if (t_ == DECIMAL) {
        return d_;
    }
    throw json_exception("非 Decimal 状态无法转换为 double.");
}
template <>
inline std::string json::cast<std::string>() {
    if (t_ == STRING) {
        return s_;
    } else {
        return *this;
    }
}

inline void json::push_back(const json &value) {
    if (t_ == NONE) {
        t_ = ARRAY;
        constructor();
    }
    if (t_ != ARRAY) {
        throw json_exception("非 Array 状态无法使用 push_back.");
    }
    m_[len()] = value;
}
inline void json::push_back(json &&value) {
    if (t_ == NONE) {
        t_ = ARRAY;
        constructor();
    }
    if (t_ != ARRAY) {
        throw json_exception("非 Array 状态无法使用 push_back.");
    }
    m_[len()] = std::move(value);
}

inline json &json::back() {
    if (t_ != ARRAY) {
        throw json_exception("非 Array 状态无法使用 back.");
    }
    return (*--m_.end()).second;
}
inline const json &json::back() const {
    if (t_ != ARRAY) {
        throw json_exception("非 Array 状态无法使用 back.");
    }
    return (*--m_.cend()).second;
}

inline void json::erase(const json_key &key) {
    switch (t_) {
        case ARRAY:
            if (key.t_ != key.INTEGRAL) {
                throw json_exception(
                    "Array 状态下只能使用整数型 json_key.");
            }
        case OBJECT:
            m_.erase(key);
            return;

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 erase().");
}

inline size_t json::size() const {
    switch (t_) {
        case ARRAY:
        case OBJECT:
            return m_.size();

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 size().");
}
inline size_t json::len() const {
    switch (t_) {
        case ARRAY:
            return m_.size() ? (*--m_.cend()).first.i_ + 1 : 0;

        case OBJECT:
            return m_.size();

        default:;
    }
    throw json_exception("非 Array 或 Object 状态无法使用 len().");
}

inline void json::fill_array(const json &sam) {
    if (t_ != ARRAY) {
        throw json_exception("非 Array 状态无法使用 fill_array.");
    }
    long long index = 0;
    for (auto iter = m_.begin(); iter != m_.end(); ++iter, ++index) {
        for (; index < iter->first.i_; ++index) {
            operator[](index) = sam;
        }
    }
}

inline void json::clear() { change_type(NONE); }

_ITERATOR(DEFINE);

inline json json::parse(const std::string &json_str) {
    return json_parser(json_str).do_it();
}

inline void json::constructor() {
    switch (t_) {
        case STRING:
            new (&s_) std::string();
            return;

        case ARRAY:
        case OBJECT:
            new (&m_) std::map<json_key, json>();
            return;

        default:;
    }
}

inline void json::destructor() {
    switch (t_) {
        case STRING:
            s_.~basic_string();
            return;

        case ARRAY:
        case OBJECT:
            m_.~map();
            return;

        default:;
    }
}

inline void json::change_type(decltype(t_) t) {
    if (t == t_) {
        return;
    }
    destructor();
    t_ = t;
    constructor();
}

template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline Int json::cast_int() {
    if (t_ == INTEGRAL) {
        return i_;
    }
    throw json_exception("非 Integral 状态无法转换.");
}

inline std::ostream &operator<<(std::ostream &out, const json &j) {
    out << static_cast<std::string>(j);
    return out;
}

}  // namespace cedar
