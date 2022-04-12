# cedar::json::fill_array

[`cedar::json`](./cedar__json.md) 成员函数

| 函数原型                                     |     |
| -------------------------------------------- | --- |
| `void fill_array(const json &sam = null());` | (1) |

填充当 `this` 为 `Array` 状态时, 因 `json_key` 的值不连续所导致的空洞, 使得实际存储数量等于逻辑存储数量, 即 `size() == len()`.

## 异常

当 `this` 不为 `Array` 状态时抛出异常.

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

    j.clear();

    j.fill_array(j[10] = 0);
    cout << j << ' ' << j.size() << ' ' << j.len() << endl;

    return 0;
}
```

输出

```
[null,null,null,3] 1 4
[null,null,null,3,null,5] 2 6
["-","-","-",3,"-",5] 6 6
[0,0,0,0,0,0,0,0,0,0,0] 11 11
```