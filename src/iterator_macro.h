#ifndef __CEDAR_ITERATOR_MACRO_HPP__
#define __CEDAR_ITERATOR_MACRO_HPP__

#define ITERATOR(act)          \
    ITERATOR_FOR_TYPE(act, , ) \
    ITERATOR_FOR_TYPE(act, reverse_, r)

#define ITERATOR_FOR_TYPE(act, type, name)           \
    ITERATOR_GROUP(act, type##iterator, name##begin) \
    ITERATOR_GROUP(act, type##iterator, name##end)

#define ITERATOR_GROUP(act, type, name)                \
    act##_ITERATOR_ITEM(type, name, )                  \
        act##_ITERATOR_ITEM(const_##type, name, const) \
            act##_ITERATOR_ITEM(const_##type, c##name, const)

#define DECLARE_ITERATOR_ITEM(type, name, limit) \
    type name() limit;

#define DEFINE_ITERATOR_ITEM(type, name, limit)                                   \
    json::type json::name() limit {                                               \
        if (t_ != ARRAY && t_ != OBJECT) {                                        \
            throw json_exception("非 Array 或 Object 状态无法调用 " #name "()."); \
        }                                                                         \
        return m_.name();                                                         \
    }

#endif