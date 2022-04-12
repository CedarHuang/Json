namespace cedar {

inline json_array::json_array(std::initializer_list<data_type::value_type> init) : data_(init) {}

inline const json_array::data_type &json_array::data() const {
    return data_;
}

inline json_object::json_object(std::initializer_list<data_type::value_type> init) : data_(init) {}

inline const json_object::data_type &json_object::data() const {
    return data_;
}

}  // namespace cedar