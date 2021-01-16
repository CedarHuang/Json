#ifndef __CEDAR_JSON_HPP__
#define __CEDAR_JSON_HPP__

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

#include "iterator_macro.h"

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
           INTEGRAL = 4,
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

struct json final {
    using null = json_null;
    using array = json_array;
    using object = json_object;

    json();

    json(bool i);

    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    json(Int i);

    json(float d);
    json(double d);

    json(const char *s);
    json(const std::string &s);
    json(std::string &&s);

    json(const json_null &);
    json(const json_array &);
    json(const json_object &);

    json(const json &other);
    json(json &&other);

    ~json();

    json &operator=(const json &other);
    json &operator=(json &&other);

    json &operator[](const json_key &key);

    json &at(const json_key &key);
    const json &at(const json_key &key) const;

    operator std::string() const;

    template <class T = std::string>
    T cast();

    void push_back(const json &value);
    void push_back(json &&value);

    json &back();
    const json &back() const;

    void erase(const json_key &key);

    void clear();

    using iterator = std::map<json_key, json>::iterator;
    using const_iterator = std::map<json_key, json>::const_iterator;
    using reverse_iterator = std::map<json_key, json>::reverse_iterator;
    using const_reverse_iterator = std::map<json_key, json>::const_reverse_iterator;

    _ITERATOR(DECLARE)

    static json parse(const std::string &json_str);

  private:
    enum {
        NONE = 1,
        BOOL = 2,
        INTEGRAL = 4,
        DOUBLE = 8,
        STRING = 16,
        ARRAY = 32,
        OBJECT = 64
    } t_;
    union {
        long long i_;
        double d_;
        std::string s_;
        std::map<json_key, json> m_;
    };

    void constructor();

    void destructor();

    void change_type(decltype(t_) t);

    template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>
    Int cast_int();

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

#include "json.inc"
#include "json_exception.inc"
#include "json_key.inc"
#include "json_parser.inc"
#include "json_utils.inc"

#endif