namespace cedar {

/* !!!!!!!!!! json_exception begin !!!!!!!!!! */
inline json_exception::json_exception(const std::string &info) : info_(info) {}
inline json_exception::json_exception(std::string &&info) : info_(std::move(info)) {}
inline json_exception::~json_exception() {}
inline const char *json_exception::what() const noexcept { return info_.c_str(); }
/* !!!!!!!!!! json_exception end !!!!!!!!!! */

/* !!!!!!!!!! json_key begin !!!!!!!!!! */
template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline json_key::json_key(Int i) : t_(INTEGRAL), i_(i) {}

inline json_key::json_key(const char *s) : t_(STRING) { new (&s_) std::string(s); }
inline json_key::json_key(const std::string &s) : t_(STRING) { new (&s_) std::string(s); }
inline json_key::json_key(std::string &&s) : t_(STRING) { new (&s_) std::string(std::move(s)); }

inline json_key::json_key(const json_key &other) : t_(NONE) { *this = other; }
inline json_key::json_key(json_key &&other) : t_(NONE) { *this = std::move(other); }

inline json_key::~json_key() { destructor(); }

inline json_key &json_key::operator=(const json_key &other) {
    change_type(other.t_);
    switch (t_) {
        case INTEGRAL:
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
        case INTEGRAL:
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
        case INTEGRAL:
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
/* !!!!!!!!!! json_key end !!!!!!!!!! */

/* !!!!!!!!!! json begin !!!!!!!!!! */
inline json::json() : t_(NONE) {}

template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type *>
inline json::json(Int i) : t_(INTEGRAL), i_(i) {}

inline json::json(bool i) : t_(BOOL), i_(i) {}

inline json::json(float d) : t_(DOUBLE), d_(d) {}
inline json::json(double d) : t_(DOUBLE), d_(d) {}

inline json::json(const char *s) : t_(STRING) { new (&s_) std::string(s); }
inline json::json(const std::string &s) : t_(STRING) { new (&s_) std::string(s); }
inline json::json(std::string &&s) : t_(STRING) { new (&s_) std::string(std::move(s)); }

inline json::json(const json_null *) : t_(NONE) {}
inline json::json(const json_array *) : t_(ARRAY) { constructor(); }
inline json::json(const json_object *) : t_(OBJECT) { constructor(); }

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

        case DOUBLE:
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

        case DOUBLE:
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
            if (static_cast<size_t>(key.i_) > m_.size()) {
                throw json_exception("只能按序创造元素.");
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

        case DOUBLE: {
            std::stringstream ss;
            ss << d_;
            ss >> s;
        } break;

        case STRING:
            s = "\"";
            s += s_;
            s += "\"";
            break;

        case ARRAY:
            s = "[";
            for (auto iter = m_.begin(); iter != m_.end(); ++iter) {
                if (iter != m_.begin()) {
                    s += ",";
                }
                s += iter->second;
            }
            s += "]";
            break;

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
    if (t_ == DOUBLE) {
        return d_;
    }
    throw json_exception("非 Double 状态无法转换为 float.");
}
template <>
inline double json::cast<double>() {
    if (t_ == DOUBLE) {
        return d_;
    }
    throw json_exception("非 Double 状态无法转换为 double.");
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
    m_[json_key(m_.size())] = value;
}
inline void json::push_back(json &&value) {
    if (t_ == NONE) {
        t_ = ARRAY;
        constructor();
    }
    if (t_ != ARRAY) {
        throw json_exception("非 Array 状态无法使用 push_back.");
    }
    m_[json_key(m_.size())] = std::move(value);
}

inline json &json::back() {
    if (t_ != ARRAY) {
        throw json_exception("非 Array 状态无法使用 push_back.");
    }
    return (*--m_.end()).second;
}
inline const json &json::back() const {
    if (t_ != ARRAY) {
        throw json_exception("非 Array 状态无法使用 push_back.");
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

inline void json::clear() { change_type(NONE); }

ITERATOR(DEFINE);

inline json json::parse(const std::string &json_str) {
    std::stringstream ss(json_str);
    std::string s;
    char c;
    while (ss >> c) {
        s += c;
    }
    return parse_object(s, 0, s.size());
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

inline json json::parse_array(const std::string &s, size_t begin, size_t end) {
    json j(json::array);
    if (begin == end) {
        return j;
    }
    bool legal = s[begin] == '[' && s[end - 1] == ']';

    size_t len = end - 1;
    for (size_t i = begin + 1; i < len && legal;) {
        auto comma_idx = find_first_punctuation(',', s, i, len);
        if (comma_idx == npos) {
            comma_idx = len;
        }

        auto left = i;
        auto right = s[left] == '[' || s[left] == '{' || s[left] == '\"'
                         ? find_first_punctuation(s[left] + (s[left] == '\"' ? 0 : 2), s, left + 1, comma_idx)
                         : comma_idx;
        if (right == npos) {
            legal = false;
            break;
        }

        if (s[left] == '[') {
            j.push_back(parse_array(s, left, right + 1));
        } else if (s[left] == '{') {
            j.push_back(parse_object(s, left, right + 1));
        } else if (s[left] == '\"') {
            j.push_back(s.substr(left + 1, right - left - 1));
        } else if (s[left] == 'n') {
            j.push_back(json());
        } else if (s[left] == 't' || s[left] == 'f') {
            j.push_back(s[left] == 't');
        } else {
            bool has_dot =
                find_first_punctuation('.', s, left, right) != npos;
            if (has_dot) {
                j.push_back(std::stod(s.substr(left)));
            } else {
                j.push_back(std::stoll(s.substr(left)));
            }
        }
        i = comma_idx + 1;
    }
    if (legal) {
        return j;
    }
    throw json_exception("不是合法的 Json.");
}

inline json json::parse_object(const std::string &s, size_t begin, size_t end) {
    json j(json::object);
    if (begin == end) {
        return j;
    }
    bool legal = s[begin] == '{' && s[end - 1] == '}';

    size_t len = end - 1;
    for (size_t i = begin + 1; i < len && legal;) {
        auto comma_idx = find_first_punctuation(',', s, i, len);
        if (comma_idx == npos) {
            comma_idx = len;
        }
        auto colon_idx = find_first_punctuation(':', s, i, comma_idx);

        std::string key = s.substr(i + 1, colon_idx - i - 2);

        auto left = colon_idx + 1;
        auto right = s[left] == '[' || s[left] == '{' || s[left] == '\"'
                         ? find_first_punctuation(s[left] + (s[left] == '\"' ? 0 : 2), s, left + 1, comma_idx)
                         : comma_idx;
        if (right == npos) {
            legal = false;
            break;
        }

        if (s[left] == '[') {
            j[std::move(key)] = parse_array(s, left, right + 1);
        } else if (s[left] == '{') {
            j[std::move(key)] = parse_object(s, left, right + 1);
        } else if (s[left] == '\"') {
            j[std::move(key)] = s.substr(left + 1, right - left - 1);
        } else if (s[left] == 'n') {
            j[std::move(key)];
        } else if (s[left] == 't' || s[left] == 'f') {
            j[std::move(key)] = s[left] == 't';
        } else {
            bool has_dot =
                find_first_punctuation('.', s, left, right) != npos;
            if (has_dot) {
                j[std::move(key)] = std::stod(s.substr(left));
            } else {
                j[std::move(key)] = std::stoll(s.substr(left));
            }
        }
        i = comma_idx + 1;
    }
    if (legal) {
        return j;
    }
    throw json_exception("不是合法的 Json.");
}

inline size_t json::find_first_punctuation(char punctuation, const std::string s, size_t begin, size_t end) {
    bool left_qm = false;
    int count = 0;
    for (size_t i = begin; i < end; ++i) {
        if (s[i] == punctuation && count == 0) {
            return i;
        } else if (s[i] == '{' || s[i] == '[' || (s[i] == '\"' && !left_qm)) {
            ++count;
            if (s[i] == '\"') {
                left_qm = true;
            }
        } else if (s[i] == '}' || s[i] == ']' || (s[i] == '\"' && left_qm)) {
            --count;
            if (s[i] == '\"') {
                left_qm = false;
            }
        }
    }
    return npos;
}

inline std::ostream &operator<<(std::ostream &out, const json &j) {
    out << static_cast<std::string>(j);
    return out;
}
/* !!!!!!!!!! json end !!!!!!!!!! */

}  // namespace cedar
