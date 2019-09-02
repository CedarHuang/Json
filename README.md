# Json
基于 C++11 的一个 Json 库

## 如何使用

### 包含头文件
```cpp
#include "json.hpp"
```

### 生成
```cpp
cedar::json j;
j["integer"] = 2;
j["floating_point"] = 123.456;
j["bool"] = true;
j["string"] = "hello world";
j["null"] = cedar::json::null;
j["array"] = cedar::json::array;
j["object"] = cedar::json::object;
for (uint32_t i = 0; i < 5; ++i) {
    j["sequence"].push_back(i);
}
j["object2"]["key"] = "value";
```

### 输出
```cpp
std::string s = j;
```
或
```
std::cout << j << std::endl;
```
得
```json
{"array":[],"bool":true,"floating_point":123.456,"integer":2,"null":null,"object":{},"object2":{"key":"value"},"sequence":[0,1,2,3,4],"string":"hello world"}
```
格式化后即
```json
{
    "array":[],
    "bool":true,
    "floating_point":123.456,
    "integer":2,
    "null":null,
    "object":{},
    "object2":{
        "key":"value"
    },
    "sequence":[
        0, 1, 2, 3, 4
    ],
    "string":"hello world"
}
```
Ps. 输出的 Json 的 Key 按字典序排序.

### 解析
```cpp
cedar::json j = cedar::json::parse(
    "{\"array\":[],\"bool\":true,\"floating_point\":123.456,\"integer\":2,"
    "\"null\":null,\"object\":{},\"object2\":{\"key\":\"value\"},"
    "\"sequence\":[0,1,2,3,4],\"string\":\"hello world\"}"
);
```

### 取值
```cpp
std::cout << j["bool"].cast<bool>() << std::endl;
std::cout << j["integer"].cast<int>() << std::endl;
std::cout << j["floating_point"].cast<double>() << std::endl;
```
得
```
1
2
123.456
```
当然也可以获取值的字符串:
```
std::cout << j["integer"].cast() << std::endl;
```
不填写模板参数即可.

### 修改
```cpp
for (auto &i : j["sequence"]) {
    // i 的类型为 pair<cedar::json_key, cedar::json>
    // 在 Array 状态下, first 为数组下标, second 为 value
    // 在 Object 状态下, first 为 key, second 为 value
    i.second = (i.second.cast<int>() + 1) * 2;
}
std::cout << j["sequence"] << std::endl;
```
得
```
[2,4,6,8,10]
```

## 谢谢.