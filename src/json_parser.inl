namespace cedar {

inline json_parser::json_parser(const std::string &s) {
    bool in_double_quotes = false;
    for (size_t i = 0; i < s.size(); ++i) {
        bool pre_in = in_double_quotes;
        if (s[i] == '"' && !is_escape_double_quotes(s, i)) {
            in_double_quotes = !in_double_quotes;
        }
        if (!in_double_quotes && std::isspace(s[i])) {
            continue;
        }
        s_ += s[i];
        in_double_quotes_.push_back(in_double_quotes && pre_in);
    }
}

inline json json_parser::do_it() {
    if (s_.empty()) {
        return json::null();
    }
    return s_.front() == '{' ? parse_object(0, s_.size()) : parse_array(0, s_.size());
}

inline json json_parser::parse_array(size_t begin, size_t end) {
    json j((json::array()));
    if (begin == end) {
        return j;
    }
    bool legal = s_[begin] == '[' && s_[end - 1] == ']';

    size_t len = end - 1;
    for (size_t i = begin + 1; i < len && legal;) {
        auto comma_idx = find_first_punctuation(',', i, len);
        if (comma_idx == npos) {
            comma_idx = len;
        }

        auto left = i;
        auto right = s_[left] == '[' || s_[left] == '{' || s_[left] == '\"'
                         ? find_first_punctuation(s_[left] + (s_[left] == '\"' ? 0 : 2), left + 1, comma_idx)
                         : comma_idx;
        if (right == npos) {
            legal = false;
            break;
        }

        if (s_[left] == '[') {
            j.push_back(parse_array(left, right + 1));
        } else if (s_[left] == '{') {
            j.push_back(parse_object(left, right + 1));
        } else if (s_[left] == '\"') {
            j.push_back(s_.substr(left + 1, right - left - 1));
        } else if (s_[left] == 'n') {
            j.push_back(json());
        } else if (s_[left] == 't' || s_[left] == 'f') {
            j.push_back(s_[left] == 't');
        } else {
            bool has_dot =
                find_first_punctuation('.', left, right) != npos;
            if (has_dot) {
                j.push_back(std::stod(s_.substr(left)));
            } else {
                j.push_back(std::stoll(s_.substr(left)));
            }
        }
        i = comma_idx + 1;
    }
    if (legal) {
        return j;
    }
    throw json_exception("不是合法的 Json.");
}

inline json json_parser::parse_object(size_t begin, size_t end) {
    json j((json::object()));
    if (begin == end) {
        return j;
    }
    bool legal = s_[begin] == '{' && s_[end - 1] == '}';

    size_t len = end - 1;
    for (size_t i = begin + 1; i < len && legal;) {
        auto comma_idx = find_first_punctuation(',', i, len);
        if (comma_idx == npos) {
            comma_idx = len;
        }
        auto colon_idx = find_first_punctuation(':', i, comma_idx);

        std::string key = s_.substr(i + 1, colon_idx - i - 2);

        auto left = colon_idx + 1;
        auto right = s_[left] == '[' || s_[left] == '{' || s_[left] == '\"'
                         ? find_first_punctuation(s_[left] + (s_[left] == '\"' ? 0 : 2), left + 1, comma_idx)
                         : comma_idx;
        if (right == npos) {
            legal = false;
            break;
        }

        if (s_[left] == '[') {
            j[std::move(key)] = parse_array(left, right + 1);
        } else if (s_[left] == '{') {
            j[std::move(key)] = parse_object(left, right + 1);
        } else if (s_[left] == '\"') {
            j[std::move(key)] = s_.substr(left + 1, right - left - 1);
        } else if (s_[left] == 'n') {
            j[std::move(key)];
        } else if (s_[left] == 't' || s_[left] == 'f') {
            j[std::move(key)] = s_[left] == 't';
        } else {
            bool has_dot =
                find_first_punctuation('.', left, right) != npos;
            if (has_dot) {
                j[std::move(key)] = std::stod(s_.substr(left));
            } else {
                j[std::move(key)] = std::stoll(s_.substr(left));
            }
        }
        i = comma_idx + 1;
    }
    if (legal) {
        return j;
    }
    throw json_exception("不是合法的 Json.");
}

inline size_t json_parser::find_first_punctuation(char punctuation, size_t begin, size_t end) {
    bool left_qm = false;
    int count = 0;
    for (size_t i = begin; i < end; ++i) {
        if (in_double_quotes_[i]) {
            continue;
        }
        if (s_[i] == punctuation && count == 0) {
            return i;
        } else if (s_[i] == '{' || s_[i] == '[' || (s_[i] == '\"' && !left_qm)) {
            ++count;
            if (s_[i] == '\"') {
                left_qm = true;
            }
        } else if (s_[i] == '}' || s_[i] == ']' || (s_[i] == '\"' && left_qm)) {
            --count;
            if (s_[i] == '\"') {
                left_qm = false;
            }
        }
    }
    return npos;
}

inline bool json_parser::is_escape_double_quotes(const std::string &s, size_t index) {
    size_t backslash_count = 0;
    for (--index; index >= 0; --index) {
        if (s[index] != '\\') {
            break;
        }
        ++backslash_count;
    }
    return backslash_count & 1;
}

}