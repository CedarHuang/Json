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
                THROW_CANNOT_USE_KEY_(key);
            }
            return m_[key];
        case type::object:
            if (!key.is_string()) {
                THROW_CANNOT_USE_KEY_(key);
            }
            return m_[key];

        default:;
    }
    THROW_CANNOT_CALL_;
}

inline json &json::at(const json_key &key) {
    switch (t_) {
        case type::array:
            if (!key.is_integer()) {
                THROW_CANNOT_USE_KEY_(key);
            }
            return m_.at(key);
        case type::object:
            if (!key.is_string()) {
                THROW_CANNOT_USE_KEY_(key);
            }
            return m_.at(key);

        default:;
    }
    THROW_CANNOT_CALL_;
}
inline const json &json::at(const json_key &key) const {
    switch (t_) {
        case type::array:
            if (!key.is_integer()) {
                THROW_CANNOT_USE_KEY_(key);
            }
            return m_.at(key);
        case type::object:
            if (!key.is_string()) {
                THROW_CANNOT_USE_KEY_(key);
            }
            return m_.at(key);

        default:;
    }
    THROW_CANNOT_CALL_;
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

inline void json::push_back(const json &value) {
    if (is_null()) {
        t_ = type::array;
        constructor();
    }
    if (!is_array()) {
        THROW_CANNOT_CALL_;
    }
    m_[len()] = value;
}
inline void json::push_back(json &&value) {
    if (is_null()) {
        t_ = type::array;
        constructor();
    }
    if (!is_array()) {
        THROW_CANNOT_CALL_;
    }
    m_[len()] = std::move(value);
}

inline json &json::back() {
    if (!is_array()) {
        THROW_CANNOT_CALL_;
    }
    return (*--m_.end()).second;
}
inline const json &json::back() const {
    if (!is_array()) {
        THROW_CANNOT_CALL_;
    }
    return (*--m_.cend()).second;
}

inline void json::erase(const json_key &key) {
    switch (t_) {
        case type::array:
            if (!key.is_integer()) {
                THROW_CANNOT_USE_KEY_(key);
            }
        case type::object:
            m_.erase(key);
            return;

        default:;
    }
    THROW_CANNOT_CALL_;
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
    THROW_CANNOT_CALL_;
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
    THROW_CANNOT_CALL_;
}

inline void json::fill_array(const json &sam) {
    if (!is_array()) {
        THROW_CANNOT_CALL_;
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
            THROW_CANNOT_CALL_; \
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

#define CEDAR_JSON_IF_RETURN_(_type, ret) \
    if (!is_##_type()) {                  \
        THROW_CANNOT_CALL_;               \
    }                                     \
    return ret

inline bool json::get_boolean() const { CEDAR_JSON_IF_RETURN_(boolean, i_); }
inline json::integer json::get_integer() const { CEDAR_JSON_IF_RETURN_(integer, i_); }
inline json::decimal json::get_decimal() const { CEDAR_JSON_IF_RETURN_(decimal, d_); }
inline json::string json::get_string() const { CEDAR_JSON_IF_RETURN_(string, s_); }

inline bool &json::ref_boolean() { CEDAR_JSON_IF_RETURN_(boolean, reinterpret_cast<bool &>(i_)); }
inline const bool &json::ref_boolean() const { CEDAR_JSON_IF_RETURN_(boolean, reinterpret_cast<const bool &>(i_)); }
inline json::integer &json::ref_integer() { CEDAR_JSON_IF_RETURN_(integer, i_); }
inline const json::integer &json::ref_integer() const { CEDAR_JSON_IF_RETURN_(integer, i_); }
inline json::decimal &json::ref_decimal() { CEDAR_JSON_IF_RETURN_(decimal, d_); }
inline const json::decimal &json::ref_decimal() const { CEDAR_JSON_IF_RETURN_(decimal, d_); }
inline json::string &json::ref_string() { CEDAR_JSON_IF_RETURN_(string, s_); }
inline const json::string &json::ref_string() const { CEDAR_JSON_IF_RETURN_(string, s_); }

template <>
inline bool &json::cast<bool &>() { CEDAR_JSON_IF_RETURN_(boolean, reinterpret_cast<bool &>(i_)); }
template <>
inline json::integer &json::cast<json::integer &>() { CEDAR_JSON_IF_RETURN_(integer, i_); }
template <>
inline json::decimal &json::cast<json::decimal &>() { CEDAR_JSON_IF_RETURN_(decimal, d_); }
template <>
inline json::string &json::cast<json::string &>() { CEDAR_JSON_IF_RETURN_(string, s_); }
template <>
inline const std::string &json::cast<const std::string &>() const { CEDAR_JSON_IF_RETURN_(string, s_); }

template <class T>
inline T json::cast() const { return cast_int<T>(); }
template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline Int json::cast_int() const {
    switch (t_) {
        case type::null:
            return 0;

        case type::boolean:
        case type::integer:
            return i_;

        case type::decimal:
            return d_;

        default:;
    }
    THROW_CANNOT_CALL_;
}
template <>
inline bool json::cast<bool>() const {
    switch (t_) {
        case type::null:
            return false;

        case type::boolean:
        case type::integer:
            return i_;

        default:;
    }
    THROW_CANNOT_CALL_;
}
template <>
inline float json::cast<float>() const {
    switch (t_) {
        case type::null:
            return 0;

        case type::boolean:
        case type::integer:
            return static_cast<float>(i_);

        case type::decimal:
            return static_cast<float>(d_);

        default:;
    }
    THROW_CANNOT_CALL_;
}
template <>
inline double json::cast<double>() const {
    switch (t_) {
        case type::null:
            return 0;

        case type::boolean:
        case type::integer:
            return static_cast<double>(i_);

        case type::decimal:
            return d_;

        default:;
    }
    THROW_CANNOT_CALL_;
}
template <>
inline std::string json::cast<std::string>() const {
    if (is_string()) {
        return s_;
    } else {
        return dump();
    }
}

#undef CEDAR_JSON_IF_RETURN_

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

inline std::ostream &operator<<(std::ostream &out, const json &j) {
    out << j.dump();
    return out;
}

}  // namespace cedar
