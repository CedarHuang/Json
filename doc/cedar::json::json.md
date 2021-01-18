# cedar::json::json

[`cedar::json`](./cedar::json.md) 成员函数

| 函数原型                                                                                                           |     |
| ------------------------------------------------------------------------------------------------------------------ | --- |
| `json();`                                                                                                          | (1) |
| `json(bool i);`                                                                                                    | (2) |
| `template <class Int, typename std::enable_if<std::is_integral<Int>::value>::type * = nullptr>`<br/>`json(Int i);` | (3) |
| `json(float d);`<br/>`json(double d);`                                                                             | (4) |
| `json(const char *s);`<br/>`json(const std::string &s);`<br/>`json(std::string &&s);`                              | (5) |
| `json(const null &);`                                                                                              | (6) |
| `json(const array &a);`                                                                                            | (7) |
| `json(const object &o);`                                                                                           | (8) |

1) 默认构造函数, 构造 `Null` 状态的 `cedar::json`.  
2) 通过 `bool` 值构造一个 `Bool` 状态的 `cedar::json`.  
3) 通过整数类型数值构造一个 `Integral` 状态的 `cedar::json`, 内部统一使用 `long long` 存储.  
4) 通过浮点数类型数值构造一个 `Decimal` 状态的 `cedar::json`, 内部统一使用 `double` 存储.  
5) 通过字符串类型构造一个 `String` 状态的 `cedar::json`, 内部统一使用 `std::string` 存储.  
6) 通过 `cedar::json::null` 构造 `Null` 状态的 `cedar::json`.  
7) 通过 `cedar::json::array` 构造 `Array` 状态的 `cedar::json`.  
8) 通过 `cedar::json::object` 构造 `Object` 状态的 `cedar::json`.  

## 示例

```cpp
#include <iostream>

#include "src/json.hpp"

using namespace std;
using namespace cedar;

int main() {
    json j;
    j["integer"] = 2;
    j["floating_point"] = 123.456;
    j["bool"] = true;
    j["string"] = "hello world";
    j["null"] = json::null();
    j["array"] = json::array();
    j["object"] = json::object();
    for (uint32_t i = 0; i < 5; ++i) {
        j["sequence"][i] = i;
    }
    j["object2"]["key"] = "value";

    json j2 = json::object{
        {"integer", 2},
        {"floating_point", 123.456},
        {"bool", true},
        {"string", "hello world"},
        {"null", json::null()},
        {"array", json::array()},
        {"object", json::object()},
        {"sequence", json::array{0, 1, 2, 3, 4}},
        {"object2", json::object{{"key", "value"}}},
    };

    assert(j.operator string() == j2.operator string());
  
    return 0;
}
```

