namespace cedar {

inline json_exception::json_exception(const std::string &info) : info_(info) {}
inline json_exception::json_exception(std::string &&info) : info_(std::move(info)) {}
inline json_exception::~json_exception() {}
inline const char *json_exception::what() const noexcept { return info_.c_str(); }

}  // namespace cedar