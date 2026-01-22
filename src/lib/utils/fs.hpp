#pragma once

#include <chrono>
#include <span>
#include <string>
#include <string_view>
#include <sll.hpp>

namespace fs {
    struct FileInfo {
        bool exists;
        bool is_directory;
        std::chrono::duration<__int128, std::ratio<1, 1000000000>> file_modified_ms;
    };
    LL_API std::string join(const std::span<std::string> paths);
    LL_API FileInfo get_info(const std::string_view path);
    LL_API std::string read_as_string(const std::string &path);
    LL_API std::vector<uint8_t> read_as_binary(const std::string &path);
    LL_API bool write_as_string(const std::string &path, const std::string &data);
};