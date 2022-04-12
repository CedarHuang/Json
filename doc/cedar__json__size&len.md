# cedar::json::size & cedar::json::len

[`cedar::json`](./cedar__json.md) 成员函数

| 函数原型               |     |
| ---------------------- | --- |
| `size_t size() const;` | (1) |
| `size_t len() const;`  | (2) |

若 `this` 为 `String` 或 `Object` 状态, `size() == len()` 恒真.  
当 `this` 为 `String` 状态时, `size()` 与 `len()` 值为字符串长度.  
当 `this` 为 `Object` 状态时, `size()` 与 `len()` 值为容纳键值对数量.  
当 `this` 为 `Array` 状态时, 若存在空洞, 则实际存储数量小于逻辑存储数量, 此时 `size() < len()`, `size()` 值为不计空洞的实际数组长度, `len()` 为计入空洞的逻辑数组长度.

## 异常

当 `this` 不为 `String` `Array` `Object` 状态时抛出异常.

## 示例

```cpp
#include <iostream>

#include "src/json.hpp"

using namespace std;
using namespace cedar;

int main() {
    // json j;
    J j;
    j[3] = 3;
    cout << j << ' ' << j.size() << ' ' << j.len() << endl;
    j[5] = 5;
    cout << j << ' ' << j.size() << ' ' << j.len() << endl;
    j.fill_array("-");
    cout << j << ' ' << j.size() << ' ' << j.len() << endl;

    return 0;
}
```

输出

```
[null,null,null,3] 1 4
[null,null,null,3,null,5] 2 6
["-","-","-",3,"-",5] 6 6
```