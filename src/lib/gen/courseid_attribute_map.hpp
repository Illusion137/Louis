#pragma once
#include <frozen/unordered_map.h>
#include <frozen/string.h>

constexpr frozen::unordered_map<frozen::string, int, 2> olaf = {
    {"19", 19},
    {"31", 31},
};
constexpr auto val = olaf.at("19");