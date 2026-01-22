#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <utils/fs.hpp>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <utils/fs.hpp>
#include <sll.hpp>

#define DEFAULT_CACHE_DIR "/Users/illusion/dev/Louis/cache/"

namespace cache {
    const static inline std::string cache_file_extension = ".che";
    LL_API std::string calculate_md5(std::string_view sv);
    LL_API std::string cache_key(const std::string_view payload);

    LL_API std::string generate_cache_file_path(const std::string key, const std::string directory = DEFAULT_CACHE_DIR);

    LL_API void clear_cache(const std::string directory = DEFAULT_CACHE_DIR);
    LL_API void clear_cache(const std::string_view payload, const std::string directory = DEFAULT_CACHE_DIR);

    LL_API std::optional<std::string> check_cache_string(const std::string_view payload, const size_t expire_ms, const std::string directory = DEFAULT_CACHE_DIR);

    LL_API std::optional<std::string_view> insert_cache(const std::string_view payload, const std::string &data, const std::string directory = DEFAULT_CACHE_DIR);
};