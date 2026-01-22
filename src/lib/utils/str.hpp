#pragma once
#include <vector>
#include <string>

static inline std::vector<std::string> split_string(const std::string &str, const std::string &delimiter){
    std::vector<std::string> splits;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        splits.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    splits.push_back(str.substr(start)); // Add the last token
    return splits;
}

static inline std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}