#include <cstdlib>
#include <unordered_set>
#include <ranges>
#include <algorithm>
#include <cctype>
#include <format>
#include <fstream>
#include <vector>
#include <glaze/glaze.hpp>
#include <napi/napi.hpp>
#include <sll.hpp>

#define GEN_DIRECTORY "/Users/illusion/dev/Louis/src/lib/gen/"
#define MAX_CONSTEXPR_ENUMS 300

template<std::ranges::input_range R, class Fn>
auto emap(R &&args, Fn fn)
{
    using T = std::ranges::range_value_t<R>;
    using V = std::invoke_result_t<Fn, T>;

    std::vector<V> result;
    result.reserve(std::ranges::size(args));
    for(const auto &arg : args) {
        result.push_back(fn(arg));
    }
    return result;
}

std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string clean_description(std::string str){
    str = replace_all(str, "\n", " ");
    return replace_all(str, "\"", "\\\"");
};

struct LLEnum {
    struct LLEnum_Value {
        std::string id;
        std::string text;
    };
    std::vector<LLEnum_Value> values;
    std::string name;
    bool eclass;
};
void generate_enum_hpp(const char *filename, const LLEnum& llenum) noexcept{
    std::string filepath = std::string{GEN_DIRECTORY} + filename;
    std::ofstream outfile;
    outfile.open(filepath);

    if(!outfile.is_open()) LL_LOG_FATAL("Failed to open file {} to generate enum {}: {}", filepath , llenum.name, filename);

    outfile << "#pragma once\n";
    outfile << "#include <frozen/unordered_map.h>\n";
    outfile << "#include <frozen/string.h>\n\n";

    outfile << "enum ";
    if(llenum.eclass) outfile << "class ";
    outfile << llenum.name << " {\n";

    for(const auto &value: llenum.values){
        outfile << "\t" << value.id << ",\n";
    }

    outfile << "};\n\n";

    std::string lowercase_enum_name = std::string{llenum.name};
    std::transform(lowercase_enum_name.begin(), lowercase_enum_name.end(), lowercase_enum_name.begin(), tolower);

    if(llenum.values.size() >= MAX_CONSTEXPR_ENUMS) outfile << "const ";
    else outfile << "constexpr ";
    outfile << std::format("frozen::unordered_map<frozen::string, {}, {}> {}_fstrmap = {{\n", llenum.name, llenum.values.size(), lowercase_enum_name);
    for(const auto &value: llenum.values){
        outfile << std::format("\t{{\"{}\", {}::{}}},\n", value.text, llenum.name, value.id);
    }
    outfile << "};\n\n";

    if(llenum.values.size() >= MAX_CONSTEXPR_ENUMS) outfile << "const ";
    else outfile << "constexpr ";
    outfile << std::format("frozen::unordered_map<{}, frozen::string, {}> {}_tstrmap = {{\n", llenum.name, llenum.values.size(), lowercase_enum_name);
    for(const auto &value: llenum.values){
        outfile << std::format("\t{{{}::{}, \"{}\"}},\n", llenum.name, value.id, value.text);
    }
    outfile << "};";

    outfile.close();

    LL_INFO("Generated Enum \"{}\" @ {}", llenum.name, filename);
}

void generate_class_nodes_hpp(const char *filename, const napi::course_catalog::CourseCatalog &courses) noexcept{
    std::string filepath = std::string{GEN_DIRECTORY} + filename;
    std::ofstream outfile;
    outfile.open(filepath);

    if(!outfile.is_open()) LL_LOG_FATAL("Failed to open file {} to generate Class Nodes", filepath);

    auto filtered_courses = courses
        | std::views::filter([](auto course) { return !course.Subject.contains("@"); });

    outfile << "#pragma once\n";
    outfile << "#include <array>\n";
    outfile << "#include \"course_nbrs.hpp\"\n";
    outfile << "#include \"course_subjects.hpp\"\n";
    outfile << "#include <sll.hpp>\n\n";

    outfile << "enum class ClassLevel {\n";
    outfile << "\tUNDERGRADUATE,\n";
    outfile << "\tGRADUATE\n";
    outfile << "};\n\n";

    outfile << "struct ClassNode {\n";
    outfile << "\tCourseSubject subject;\n";
    outfile << "\tCourseNBR nbr;\n";
    outfile << "\tstd::string title;\n";
    outfile << "\tstd::string long_title;\n";
    outfile << "\tstd::string description;\n";
    outfile << "\tfloat credits;\n";
    outfile << "\tClassLevel level;\n";
    outfile << "};\n\n";

    outfile << std::format("const static std::array<ClassNode, {}> all_courses = {{{{\n", std::distance(filtered_courses.begin(), filtered_courses.end()));
    
    for(const auto &course: filtered_courses){
        outfile << std::format("\t{{ CourseSubject::{}, CourseNBR::C{}, \"{}\", \"{}\", \"{}\", {}, ClassLevel::{} }},\n", 
            course.Subject,
            course.CatalogNbr,
            clean_description(course.Title),
            clean_description(course.LongTitle),
            clean_description(course.Description),
            course.Credits,
            course.Level);
    }
    
    outfile << "}};";

    outfile.close();

    LL_INFO("Generated Class Nodes @ {}", filename);
}

int main(int argc, char **argv){
    // TODO add --clear flag to clear rozfetch cache

    const auto campuses = napi::connect::get_campuses();
    LL_IF_LOG_FATAL(campuses.size() == 0, "Campuses must have entries");
    const LLEnum campus_enum = {
        .name = "Campus",
        .eclass = true, 
        .values = emap(campuses, [](auto campus) -> LLEnum::LLEnum_Value { 
            return {.id = campus.Value, .text = campus.Text}; 
        })
    };
    generate_enum_hpp("campus.hpp", campus_enum);

    const auto catalog_years = napi::transfer_plan::get_catalog_years();
    LL_IF_LOG_FATAL(catalog_years.size() == 0, "Catalog Years must have entries");
    const LLEnum catalog_years_enum = {
        .name = "CatalogYears",
        .eclass = true,
        .values = emap(catalog_years, [](auto catalog_year) -> LLEnum::LLEnum_Value { 
            return {.id = "Y" + catalog_year.Value, .text = catalog_year.Text}; 
        })
    };
    generate_enum_hpp("catalog_years.hpp", catalog_years_enum);
    
    const auto course_catalog = napi::course_catalog::get_course_catalog();
    LL_IF_LOG_FATAL(course_catalog.size() == 0, "Course Catalog must have entries");

    auto course_catalog_subjects = emap(course_catalog, [](auto course) { return course.Subject; })
        | std::views::filter([](std::string nbr) { return !nbr.contains("@"); });
    std::unordered_set<std::string> course_catalog_subject_set(course_catalog_subjects.begin(), course_catalog_subjects.end());
    const LLEnum course_subject_enum = {
        .name = "CourseSubject",
        .eclass = true,
        .values = emap(course_catalog_subject_set, [](auto subject) -> LLEnum::LLEnum_Value {
            return {.id = subject, .text = subject}; 
        })
    };

    std::vector<std::string> course_catalog_nbrs = emap(course_catalog, [](auto course) { return course.CatalogNbr; });
    std::unordered_set<std::string> course_catalog_nbrs_set(course_catalog_nbrs.begin(), course_catalog_nbrs.end());
    const LLEnum course_nbr_enum = {
        .name = "CourseNBR",
        .eclass = true,
        .values = emap(course_catalog_nbrs_set, [](auto nbr) -> LLEnum::LLEnum_Value { 
            return {.id = "C" + nbr, .text = nbr}; 
        })
    };
    generate_enum_hpp("course_subjects.hpp", course_subject_enum);
    generate_enum_hpp("course_nbrs.hpp", course_nbr_enum);

    generate_class_nodes_hpp("courses.hpp", course_catalog);

    return EXIT_SUCCESS;
}