# cedar::json

定义于头文件 [`"json.hpp"`](./README.md)

## 成员类型

| 成员类型                 | 定义                                                             |
| ------------------------ | ---------------------------------------------------------------- |
| `null`                   | `cedar::json_null`                                               |
| `array`                  | `cedar::json_array`                                              |
| `object`                 | `cedar::json_object`                                             |
| `iterator`               | `std::map<cedar::json_key, cedar::json>::iterator`               |
| `const_iterator`         | `std::map<cedar::json_key, cedar::json>::const_iterator`         |
| `reverse_iterator`       | `std::map<cedar::json_key, cedar::json>::reverse_iterator`       |
| `const_reverse_iterator` | `std::map<cedar::json_key, cedar::json>::const_reverse_iterator` |

| 成员函数                             |                                 |
| ------------------------------------ | ------------------------------- |
| [(构造函数)](./cedar::json::json.md) | 构造 `json`                     |
| (析构函数)                           | 析构 `json`                     |
| `operator=`                          | 赋值给 `json`                   |
| `parse` [静态]                       | 通过解析 Json 字符串构造 `json` |

| 元素访问                                     |                                  |
| -------------------------------------------- | -------------------------------- |
| [`at`](./cedar::json::at.md)                 | 访问指定的元素，同时进行越界检查 |
| [`operator[]`](./cedar::json::operator[].md) | 访问或插入指定的元素             |
| `back`                                       | 访问最后的元素                   |
| `cast`                                       | 返回转换后的 `json` 值           |

| 迭代器                 |                          |
| ---------------------- | ------------------------ |
| `begin`<br/>`cbegin`   | 返回指向起始的迭代器     |
| `end`<br/>`send`       | 返回指向末尾的迭代器     |
| `rbegin`<br/>`crbegin` | 返回指向起始的逆向迭代器 |
| `rend`<br/>`crend`     | 返回指向末尾的逆向迭代器 |
|                        |                          |

| 容量   |                      |
| ------ | -------------------- |
| `size` | 返回真实容纳的元素数 |
| `len`  | 返回逻辑容纳的元素数 |

| 修改器       |                                         |
| ------------ | --------------------------------------- |
| `clear`      | 清除内容, 重置回 `Null` 状态            |
| `erase`      | 擦除元素                                |
| `push_back`  | 将元素添加到 `Array` 末尾               |
| `fill_array` | 填充 `Array` 空洞, 使 `size() == len()` |

| 非成员函数   |                      |
| ------------ | -------------------- |
| `operator<<` | 执行 `json` 的流输出 |

