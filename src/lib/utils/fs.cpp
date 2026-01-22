#include "fs.hpp"
#include <cstdint>
#include <fstream>
#include <ios>
#include <sstream>
#include <filesystem>
#include <vector>

std::string fs::join(const std::span<std::string> paths){
    std::filesystem::path joined;
    for(const auto &path: paths) {
        joined /= path;
    }
    return joined.string();
}

fs::FileInfo fs::get_info(const std::string_view path){
    const bool exists = std::filesystem::exists(path);
    if(!exists){
        return {
            .exists = exists,
            .is_directory = false,
            .file_modified_ms = {},
        };
    }
    return {
        .exists = exists,
        .is_directory = std::filesystem::is_directory(path),
        .file_modified_ms = std::filesystem::last_write_time(path).time_since_epoch(),
    };
}

std::string fs::read_as_string(const std::string &path){
    std::ifstream file{path};
    if (!file.is_open()) return "";

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();
    return buffer.str();
}

std::vector<uint8_t> fs::read_as_binary(const std::string &path){
    std::ifstream file{path, std::ios::in | std::ios::binary};
    if (!file.is_open()) return std::vector<uint8_t>{};

    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});

    file.close();
    return buffer;
}

bool fs::write_as_string(const std::string &path, const std::string &data){
    std::ofstream file{path};
    if(!file.is_open()) return false;
    file << data;
    file.close();
    return true;
}