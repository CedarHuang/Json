#ifndef __CEDAR_ITERATOR_MACRO_HPP__
#define __CEDAR_ITERATOR_MACRO_HPP__

#define _ITERATOR(act)          \
    _ITERATOR_FOR_TYPE(act, , ) \
    _ITERATOR_FOR_TYPE(act, reverse_, r)

#define _ITERATOR_FOR_TYPE(act, type, name)           \
    _ITERATOR_GROUP(act, type##iterator, name##begin) \
    _ITERATOR_GROUP(act, type##iterator, name##end)

#define _ITERATOR_GROUP(act, type, name)                  \
    _##act##_ITERATOR_ITEM(type, name, )                  \
        _##act##_ITERATOR_ITEM(const_##type, name, const) \
            _##act##_ITERATOR_ITEM(const_##type, c##name, const)

#define _DECLARE_ITERATOR_ITEM(type, name, limit) \
    type name() limit;

#define _DEFINE_ITERATOR_ITEM(type, name, limit)                                  \
    inline json::type json::name() limit {                                        \
        if (t_ != ARRAY && t_ != OBJECT) {                                        \
            throw json_exception("非 Array 或 Object 状态无法调用 " #name "()."); \
        }                                                                         \
        return m_.name();                                                         \
    }

#endif