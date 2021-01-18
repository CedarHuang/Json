# cedar::json::operator[]

[`cedar::json`](./cedar::json.md) 成员函数

| 函数原型                                |     |
| --------------------------------------- | --- |
| `json &operator[](const json_key &key)` |     |

取 `json_key` 对应 `json`, 若无对应 `json`, 则默认构造.  
若 `this` 为 `Array` 状态, 则 `json_key` 必须为 `Integer` 状态.  
若 `this` 为 `Object` 状态, 则 `json_key` 必须为 `String` 状态.  
若 `this` 为 `Null` 状态, 则若 `json_key` 为 `Integer` 状态, `this` 转变为 `Array` 状态, 若 `json_key` 为 `String` 状态, `this` 转变为 `Object` 状态.

当 `this` 为 `Array` 状态时, 若 `json_key` 的值不连续, 则会形成空洞, 实际存储数量小于逻辑存储数量, 此时 `size() < len()`, 可使用 `fill_array` 填充空洞 .

## 异常

当 `this` 不为 `Array` `Ojbect` `Null` 状态时,  
当 `this` 为 `Array` 而 `json_key` 不为 `Integer` 时,  
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
    cout << j << endl;

    json j2;
    j2[3] = 3;
    cout << j2 << ' ' << j2.size() << ' ' << j2.len() << endl;
    j2[5] = 5;
    cout << j2 << ' ' << j2.size() << ' ' << j2.len() << endl;
    j2.fill_array(0);
    cout << j2 << ' ' << j2.size() << ' ' << j2.len() << endl;

    return 0;
}
```

输出

```
{"int":1}
[null,null,null,3] 1 4
[null,null,null,3,null,5] 2 6
[0,0,0,3,0,5] 6 6
```

