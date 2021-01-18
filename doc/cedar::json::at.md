# cedar::json::at

[`cedar::json`](./cedar::json.md) 成员函数

| 函数原型                                     |     |
| -------------------------------------------- | --- |
| `json &at(const json_key &key);`             | (1) |
| `const json &at(const json_key &key) const;` | (2) |

取 `json_key` 对应 `json`, 若无对应 `json`, 则抛出 `std::out_of_range` 类型异常。
若 `this` 为 `Array` 状态, 则 `json_key` 必须为 `Integral` 状态.  
若 `this` 为 `Object` 状态, 则 `json_key` 必须为 `String` 状态.  

## 异常

当 `this` 不为 `Array` `Ojbect` 状态时,  
当 `this` 为 `Array` 而 `json_key` 不为 `Integral` 时,  
当 `this` 为 `Object` 而 `json_key` 不为 `String` 时,  
抛出异常.

## 示例

```cpp
#include <iostream>

#include "src/json.hpp"

using namespace std;
using namespace cedar;

int main() {
    json j;
    j["int"] = 1;
    cout << j.at("int") << endl;

    json j2;
    j2[3] = 3;
    cout << j2.at(3) << endl;

    return 0;
}
```

输出

```
1
3
```

