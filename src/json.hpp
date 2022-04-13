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

struct json_exception final : std::exception {
    explicit json_exception(const std::string &info);
    explicit json_exception(std::string &&info);
    virtual ~json_exception() override;
    virtual const char *what() const noexcept override;

  private:
    std::string info_;
};

struct json_key final {
    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    json_key(Int i);

    json_key(const char *s);
    json_key(const std::string &s);
    json_key(std::string &&s);

    json_key(const json_key &other);
    json_key(json_key &&other);

    ~json_key();

    json_key &operator=(const json_key &other);
    json_key &operator=(json_key &&other);

    bool operator<(const json_key &other) const;

    operator std::string() const;

  private:
    enum { NONE = 1,
           INTEGER = 4,
           STRING = 16
    } t_;
    union {
        long long i_;
        std::string s_;
    };

    void constructor();

    void destructor();

    void change_type(decltype(t_) t);

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

using J = struct json final {
#define CEDAR_JSON_USING_(long_, short_, target_) \
    using long_ = target_;                        \
    using short_ = long_

    CEDAR_JSON_USING_(null, N, json_null);
    CEDAR_JSON_USING_(array, A, json_array);
    CEDAR_JSON_USING_(object, O, json_object);

    CEDAR_JSON_USING_(integer, I, long long);
    CEDAR_JSON_USING_(decimal, D, double);
    CEDAR_JSON_USING_(string, S, std::string);
#undef CEDAR_JSON_USING_

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
    json(json &&other);

    ~json();

    json &operator=(const json &other);
    json &operator=(json &&other);

    json &operator[](const json_key &key);

    json &at(const json_key &key);
    const json &at(const json_key &key) const;

    operator std::string() const;

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

  private:
    enum {
        NONE = 1,
        BOOL = 2,
        INTEGER = 4,
        DECIMAL = 8,
        STRING = 16,
        ARRAY = 32,
        OBJECT = 64
    } t_;
    union {
        integer i_;
        decimal d_;
        string s_;
        std::map<json_key, json> m_;
    };

    void constructor();

    void destructor();

    void change_type(decltype(t_) t);

    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    Int cast_int() const;

    friend std::ostream &operator<<(std::ostream &out, const json &j);
};

struct json_parser final {
  public:
    json_parser(const std::string &s);

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

}  // namespace cedar

#include "json.inl"
#include "json_exception.inl"
#include "json_key.inl"
#include "json_parser.inl"
#include "json_utils.inl"

#undef CEDAR_JSON_ITERATOR_

#endif