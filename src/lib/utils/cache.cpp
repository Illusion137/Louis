#include <cstdint>
#include <iomanip>
#include <memory>
#include <openssl/evp.h>
#include <sstream>
#include <ostream>
#include <chrono>
#include <filesystem>
#include <span>
#include "cache.hpp"

std::unique_ptr<uint8_t[]> md5_hash(const unsigned char *data, const size_t length, uint32_t &md_length) {
    std::unique_ptr<uint8_t[]> md = nullptr;
    EVP_MD_CTX *ctx = nullptr;
    const EVP_MD *md_type = EVP_md5();
    if (!md_type) return nullptr;

    md_length = EVP_MD_size(md_type);
    if (md_length <= 0) return nullptr;

    md = std::make_unique<uint8_t[]>(md_length);
    if (md == nullptr) return nullptr;

    ctx = EVP_MD_CTX_new();
    if (!ctx) return NULL;

    if (EVP_DigestInit_ex(ctx, md_type, NULL) != 1) {
        EVP_MD_CTX_free(ctx);
        return NULL;
    }

    if (EVP_DigestUpdate(ctx, data, length) != 1) {
        EVP_MD_CTX_free(ctx);
        return NULL;
    }

    if (EVP_DigestFinal_ex(ctx, md.get(), NULL) != 1) {
        EVP_MD_CTX_free(ctx);
        return NULL;
    }

    EVP_MD_CTX_free(ctx);

    return md;
}
std::string cache::calculate_md5(std::string_view sv){
    uint32_t md5_len;
    std::unique_ptr<uint8_t[]> hash = md5_hash(reinterpret_cast<const uint8_t*>(sv.data()), sv.size(), md5_len);

    if(hash == nullptr) return "";

    std::ostringstream stream;
    stream << std::hex << std::setfill('0');
    for (int i = 0; i < md5_len; i++) {
        stream << std::setw(2) << static_cast<uint32_t>(hash[i]);
    }
    return stream.str();
}

std::string cache::cache_key(const std::string_view payload){
    return calculate_md5(payload);
}

std::string cache::generate_cache_file_path(const std::string key, const std::string directory) {
    std::string filename = key + cache_file_extension;
    std::array<std::string, 2> paths = {directory, filename};
    return fs::join(paths);
}

void cache::clear_cache(const std::string directory){
    std::filesystem::directory_iterator it{directory};
    for(const auto &path : it){
        if(!path.is_regular_file()) continue;
        std::filesystem::remove(path);
    }
}
void cache::clear_cache(const std::string_view payload, const std::string directory){
    const std::string file_path = generate_cache_file_path(cache_key(payload), directory);
    std::filesystem::remove(file_path);
}

std::optional<std::string> cache::check_cache_string(const std::string_view payload, const size_t expire_ms, const std::string directory){
    const std::string file_path = generate_cache_file_path(cache_key(payload), directory);
    const auto stats_result = fs::get_info(file_path);
    if(!stats_result.exists) return {};
    const auto now = std::chrono::system_clock::now();
    if(stats_result.file_modified_ms + std::chrono::milliseconds{expire_ms} < now.time_since_epoch()) {
        std::filesystem::remove(file_path);
        return {};
    }
    const std::string data = fs::read_as_string(file_path);
    return data;
}

std::optional<std::string_view> cache::insert_cache(const std::string_view payload, const std::string &data, const std::string directory){
    const bool write_result = fs::write_as_string(generate_cache_file_path(
        cache_key(payload), directory),
        data);
    if(write_result == false) return {};
    return data;
}