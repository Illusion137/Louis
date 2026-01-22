#pragma once
#include <frozen/unordered_map.h>
#include <frozen/string.h>

enum CourseAttributes: uint64_t {
    COURSE_ATTRIBUTE_NONE = 0,
    COURSE_ATTRIBUTE_DIVERSITY_ = 1<0,
    COURSE_ATTRIBUTE_GENERAL_STUDIES_ = 1<1,
    COURSE_ATTRIBUTE_INCLUSIVE_PERSPECTIVES_ = 1<2,
    COURSE_ATTRIBUTE_LIBERAL_STUDIES_ = 1<3
};

constexpr frozen::unordered_map<frozen::string, CourseAttributes, 2> courseattribute_fstrmap = {
    {"19", CourseAttributes::COURSE_ATTRIBUTE_DIVERSITY_},
    {"31", CourseAttributes::COURSE_ATTRIBUTE_DIVERSITY_},
};