#ifndef CEDAR_JSON_HPP_
#define CEDAR_JSON_HPP_

#include <cmath>
#include <exception>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace cedar {

struct json_key final {
    using integer = long long;
    using string = std::string;

    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    json_key(Int i);

    json_key(const char *s);
    json_key(const std::string &s);
    json_key(std::string &&s);

    json_key(const json_key &other);
    json_key(json_key &&other) noexcept;

    ~json_key();

    json_key &operator=(const json_key &other);
    json_key &operator=(json_key &&other) noexcept;

    bool operator<(const json_key &other) const;

    std::string dump() const;

    template <class T, typename std::enable_if<std::is_lvalue_reference<T>::value && !std::is_const<typename std::remove_reference<T>::type>::value>::type * = nullptr>
    T cast() = delete;

    template <class T = std::string>
    T cast() const;

    enum class type {
        null = 1,
        integer = 4,
        string = 16
    };

    type get_type() const;
    bool is_null() const;
    bool is_integer() const;
    bool is_string() const;

private:
    type t_;
    union {
        long long i_;
        std::string s_;
    };

    void constructor();

    void destructor();

    void change_type(type t);

    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    Int cast_int() const;

    friend struct json;
    friend std::ostream &operator<<(std::ostream &out, const json_key &j);
};

struct json;

struct json_null {
};
struct json_array {
  public:
    using data_type = std::vector<json>;

    json_array() = default;
    json_array(std::initializer_list<data_type::value_type> init);

    const data_type &data() const;

  private:
    data_type data_;
};
struct json_object {
  public:
    using data_type = std::map<json_key, json>;

    json_object() = default;
    json_object(std::initializer_list<data_type::value_type> init);

    const data_type &data() const;

  private:
    data_type data_;
};

struct json final {
    using null = json_null;
    using array = json_array;
    using object = json_object;

    using boolean = long long;
    using integer = long long;
    using decimal = double;
    using string = std::string;

    json();

    json(bool i);

    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    json(Int i);

    json(float d);
    json(double d);

    json(const char *s);
    json(const std::string &s);
    json(std::string &&s);

    json(const null &);
    json(const array &a);
    json(const object &o);

    json(const json &other);
    json(json &&other) noexcept;

    ~json();

    json &operator=(const json &other);
    json &operator=(json &&other) noexcept;

    json &operator[](const json_key &key);

    json &at(const json_key &key);
    const json &at(const json_key &key) const;

    std::string dump() const;

    template <class T, typename std::enable_if<std::is_lvalue_reference<T>::value && !std::is_const<typename std::remove_reference<T>::type>::value>::type * = nullptr>
    T cast() = delete;

    template <class T = std::string>
    T cast() const;

    void push_back(const json &value);
    void push_back(json &&value);

    json &back();
    const json &back() const;

    void erase(const json_key &key);

    size_t size() const;
    size_t len() const;

    void fill_array(const json &sam = null());

    void clear();

    using iterator = std::map<json_key, json>::iterator;
    using const_iterator = std::map<json_key, json>::const_iterator;
    using reverse_iterator = std::map<json_key, json>::reverse_iterator;
    using const_reverse_iterator = std::map<json_key, json>::const_reverse_iterator;

#define CEDAR_JSON_ITERATOR_(act)                                         \
    CEDAR_JSON_ITERATOR_##act##_(iterator, begin, );                      \
    CEDAR_JSON_ITERATOR_##act##_(const_iterator, begin, const);           \
    CEDAR_JSON_ITERATOR_##act##_(const_iterator, cbegin, const);          \
    CEDAR_JSON_ITERATOR_##act##_(iterator, end, );                        \
    CEDAR_JSON_ITERATOR_##act##_(const_iterator, end, const);             \
    CEDAR_JSON_ITERATOR_##act##_(const_iterator, cend, const);            \
    CEDAR_JSON_ITERATOR_##act##_(reverse_iterator, rbegin, );             \
    CEDAR_JSON_ITERATOR_##act##_(const_reverse_iterator, rbegin, const);  \
    CEDAR_JSON_ITERATOR_##act##_(const_reverse_iterator, crbegin, const); \
    CEDAR_JSON_ITERATOR_##act##_(reverse_iterator, rend, );               \
    CEDAR_JSON_ITERATOR_##act##_(const_reverse_iterator, rend, const);    \
    CEDAR_JSON_ITERATOR_##act##_(const_reverse_iterator, crend, const)

#define CEDAR_JSON_ITERATOR_DECLARE_(type, name, limit) \
    type name() limit

    CEDAR_JSON_ITERATOR_(DECLARE);
#undef CEDAR_JSON_ITERATOR_DECLARE_

    static json parse(const std::string &json_str);

    enum class type {
        null = 1,
        boolean = 2,
        integer = 4,
        decimal = 8,
        string = 16,
        array = 32,
        object = 64
    };

    type get_type() const;
    bool is_null() const;
    bool is_boolean() const;
    bool is_integer() const;
    bool is_decimal() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;

    bool get_boolean() const;
    integer get_integer() const;
    decimal get_decimal() const;
    string get_string() const;

    bool &ref_boolean();
    const bool &ref_boolean() const;
    integer &ref_integer();
    const integer &ref_integer() const;
    decimal &ref_decimal();
    const decimal &ref_decimal() const;
    string &ref_string();
    const string &ref_string() const;

  private:
    type t_;
    union {
        integer i_;
        decimal d_;
        string s_;
        std::map<json_key, json> m_;
    };

    void constructor();

    void destructor();

    void change_type(type t);

    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    Int cast_int() const;

    friend std::ostream &operator<<(std::ostream &out, const json &j);
};

struct json_parser final {
  public:
    explicit json_parser(const std::string &s);

    ~json_parser() = default;

    json do_it();

  private:
    json parse_array(size_t begin, size_t end);

    json parse_object(size_t begin, size_t end);

    size_t find_first_punctuation(char punctuation, size_t begin, size_t end);

    static bool is_escape_double_quotes(const std::string &s, size_t index);

  private:
    std::string s_;
    std::vector<bool> in_double_quotes_;
    std::map<size_t, size_t> pair_index_;

    static constexpr size_t npos = -1;

    friend json;
};

struct json_exception final : std::runtime_error {
    explicit json_exception(const char *info);
    explicit json_exception(const std::string &info);

    static json_exception cannot_call(const char *func, json::type type);
    static json_exception cannot_use_key(const char *func, json::type json_type, json_key::type key_type);

#define THROW_CANNOT_CALL_ throw json_exception::cannot_call(__FUNCTION__, static_cast<json::type>(get_type()))
#define THROW_CANNOT_USE_KEY_(key) throw json_exception::cannot_use_key(__FUNCTION__, get_type(), key.get_type())
#define THROW_ILLEGAL_JSON throw json_exception("Illegal Json.")

  private:
    static std::string type_name(json::type type);
};

}  // namespace cedar

#include "json.inl"
#include "json_exception.inl"
#include "json_key.inl"
#include "json_parser.inl"
#include "json_utils.inl"

#undef CEDAR_JSON_ITERATOR_
#undef THROW_CANNOT_CALL_
#undef THROW_CANNOT_USE_KEY_
#undef THROW_ILLEGAL_JSON

#endif