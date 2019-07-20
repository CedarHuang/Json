#ifndef __CEDAR_JSON_HPP__
#define __CEDAR_JSON_HPP__

#include <exception>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace cedar {

struct json_exception final : std::exception {
    explicit json_exception(const std::string &info) : info_(info) {}
    explicit json_exception(std::string &&info) : info_(std::move(info)) {}
    virtual ~json_exception() override {}
    virtual const char *what() const noexcept override { return info_.c_str(); }

  private:
    std::string info_;
};

struct json_key final {
    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    json_key(Int i) : t_(INTEGRAL), i_(i) {}

    json_key(const char *s) : t_(STRING) { new (&s_) std::string(s); }
    json_key(const std::string &s) : t_(STRING) { new (&s_) std::string(s); }
    json_key(std::string &&s) : t_(STRING) { new (&s_) std::string(std::move(s)); }

    json_key(const json_key &other) : t_(NONE) { *this = other; }
    json_key(json_key &&other) : t_(NONE) { *this = std::move(other); }
    ~json_key() { destructor(); }

    json_key &operator=(const json_key &other) {
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

    json_key &operator=(json_key &&other) {
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

    bool operator<(const json_key &other) const {
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

    operator std::string() const {
        if (t_ == NONE) {
            throw json_exception("无法将 None 状态下的 json_key 转化为 std::string.");
        }
        std::string s;
        s += t_ == STRING ? "\"" : "";
        s += t_ == STRING ? s_ : std::to_string(i_);
        s += t_ == STRING ? "\"" : "";
        return s;
    }

  private:
    enum { NONE = 1,
           INTEGRAL = 4,
           STRING = 16
    } t_;
    union {
        long long i_;
        std::string s_;
    };

    void constructor() {
        if (t_ == STRING) {
            new (&s_) std::string();
        }
    }

    void destructor() {
        if (t_ == STRING) {
            s_.~basic_string();
        }
    }

    void change_type(decltype(t_) t) {
        if (t == t_) {
            return;
        }
        destructor();
        t_ = t;
        constructor();
    }

    friend struct json;
    friend std::ostream &operator<<(std::ostream &out, const json_key &j) {
        out << static_cast<std::string>(j);
        return out;
    }
};

struct json final {
    static constexpr struct json_null {
    } *null = nullptr;
    static constexpr struct json_array {
    } *array = nullptr;
    static constexpr struct json_object {
    } *object = nullptr;

    json() : t_(NONE) {}

    json(bool i) : t_(BOOL), i_(i) {}

    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    json(Int i) : t_(INTEGRAL), i_(i) {}

    json(float d) : t_(DOUBLE), d_(d) {}
    json(double d) : t_(DOUBLE), d_(d) {}

    json(const char *s) : t_(STRING) { new (&s_) std::string(s); }
    json(const std::string &s) : t_(STRING) { new (&s_) std::string(s); }
    json(std::string &&s) : t_(STRING) { new (&s_) std::string(std::move(s)); }

    json(const json_null *) : t_(NONE) {}
    json(const json_array *) : t_(VECTOR) { constructor(); }
    json(const json_object *) : t_(MAP) { constructor(); }

    json(const json &other) : t_(NONE) { *this = other; }
    json(json &&other) : t_(NONE) { *this = std::move(other); }

    ~json() { destructor(); }

    json &operator=(const json &other) {
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

            case VECTOR:
            case MAP:
                m_ = other.m_;
                break;

            default:;
        }
        return *this;
    }

    json &operator=(json &&other) {
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

            case VECTOR:
            case MAP:
                m_ = std::move(other.m_);
                break;

            default:;
        }
        other.change_type(NONE);
        return *this;
    }

    json &operator[](const json_key &key) {
        if (t_ == NONE) {
            t_ = key.t_ == key.INTEGRAL ? VECTOR : MAP;
            constructor();
        }
        switch (t_) {
            case VECTOR:
                if (key.t_ != key.INTEGRAL) {
                    throw json_exception("Vector 状态下只能使用整数型 json_key.");
                }
                if (static_cast<size_t>(key.i_) > m_.size()) {
                    throw json_exception("只能按序创造元素.");
                }
                return m_[key];
            case MAP:
                if (key.t_ != key.STRING) {
                    throw json_exception("Map 状态下只能使用字符串型 json_key.");
                }
                return m_[key];

            default:;
        }
        throw json_exception("非 Vector 或 Map 状态无法使用 operator[].");
    }

    json &at(const json_key &key) {
        switch (t_) {
            case VECTOR:
                if (key.t_ != key.INTEGRAL) {
                    throw json_exception("Vector 状态下只能使用整数型 json_key.");
                }
                return m_.at(key);
            case MAP:
                if (key.t_ != key.STRING) {
                    throw json_exception("Map 状态下只能使用字符串型 json_key.");
                }
                return m_.at(key);

            default:;
        }
        throw json_exception("非 Vector 或 Map 状态无法使用 at().");
    }

    const json &at(const json_key &key) const {
        switch (t_) {
            case VECTOR:
                if (key.t_ != key.INTEGRAL) {
                    throw json_exception("Vector 状态下只能使用整数型 json_key.");
                }
                return m_.at(key);
            case MAP:
                if (key.t_ != key.STRING) {
                    throw json_exception("Map 状态下只能使用字符串型 json_key.");
                }
                return m_.at(key);

            default:;
        }
        throw json_exception("非 Vector 或 Map 状态无法使用 at().");
    }

    operator std::string() const {
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

            case VECTOR:
                s = "[";
                for (auto iter = m_.begin(); iter != m_.end(); ++iter) {
                    if (iter != m_.begin()) {
                        s += ",";
                    }
                    s += iter->second;
                }
                s += "]";
                break;

            case MAP:
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

    void push_back(const json &value) {
        if (t_ == NONE) {
            t_ = VECTOR;
            constructor();
        }
        if (t_ != VECTOR) {
            throw json_exception("非 Vector 状态无法使用 push_back.");
        }
        m_[json_key(m_.size())] = value;
    }

    void push_back(json &&value) {
        if (t_ == NONE) {
            t_ = VECTOR;
            constructor();
        }
        if (t_ != VECTOR) {
            throw json_exception("非 Vector 状态无法使用 push_back.");
        }
        m_[json_key(m_.size())] = std::move(value);
    }

    void erase(const json_key &key) {
        switch (t_) {
            case VECTOR:
                if (key.t_ != key.INTEGRAL) {
                    throw json_exception(
                        "Vector 状态下只能使用整数型 json_key.");
                }
            case MAP:
                m_.erase(key);
                return;

            default:;
        }
        throw json_exception("非 Vector 或 Map 状态无法使用 erase().");
    }

    void clear() { change_type(NONE); }

    using iterator = std::map<json_key, json>::iterator;
    using const_iterator = std::map<json_key, json>::const_iterator;
    using reverse_iterator = std::map<json_key, json>::reverse_iterator;
    using const_reverse_iterator = std::map<json_key, json>::const_reverse_iterator;

#define DEFINE_ITERATOR          \
    DEFINE_ITERATOR_FOR_TYPE(, ) \
    DEFINE_ITERATOR_FOR_TYPE(reverse_, r)

#define DEFINE_ITERATOR_FOR_TYPE(type, name)           \
    DEFINE_ITERATOR_GROUP(type##iterator, name##begin) \
    DEFINE_ITERATOR_GROUP(type##iterator, name##end)

#define DEFINE_ITERATOR_GROUP(type, name)           \
    DEFINE_ITERATOR_ITEM(type, name, )              \
    DEFINE_ITERATOR_ITEM(const_##type, name, const) \
    DEFINE_ITERATOR_ITEM(const_##type, c##name, const)

#define DEFINE_ITERATOR_ITEM(type, name, limit)                                 \
    type name() limit {                                                         \
        if (t_ != VECTOR && t_ != MAP) {                                        \
            throw json_exception("非 VECTOR 或 MAP 状态无法调用 " #name "()."); \
        }                                                                       \
        return m_.name();                                                       \
    }

    DEFINE_ITERATOR

    static json parse(const std::string &json_str) {
        std::stringstream ss(json_str);
        std::string s;
        char c;
        while (ss >> c) {
            s += c;
        }
        return parse_object(s, 0, s.size());
    }

  private:
    enum {
        NONE = 1,
        BOOL = 2,
        INTEGRAL = 4,
        DOUBLE = 8,
        STRING = 16,
        VECTOR = 32,
        MAP = 64
    } t_;
    union {
        long long i_;
        double d_;
        std::string s_;
        std::map<json_key, json> m_;
    };

    static constexpr size_t npos = -1;

    void constructor() {
        switch (t_) {
            case STRING:
                new (&s_) std::string();
                return;

            case VECTOR:
            case MAP:
                new (&m_) std::map<json_key, json>();
                return;

            default:;
        }
    }

    void destructor() {
        switch (t_) {
            case STRING:
                s_.~basic_string();
                return;

            case VECTOR:
            case MAP:
                m_.~map();
                return;

            default:;
        }
    }

    void change_type(decltype(t_) t) {
        if (t == t_) {
            return;
        }
        destructor();
        t_ = t;
        constructor();
    }

    static json parse_array(const std::string &s, size_t begin, size_t end) {
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

    static json parse_object(const std::string &s, size_t begin, size_t end) {
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

    static size_t find_first_punctuation(char punctuation, const std::string s, size_t begin, size_t end) {
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

    friend std::ostream &operator<<(std::ostream &out, const json &j) {
        out << static_cast<std::string>(j);
        return out;
    }
};

}  // namespace cedar

#endif