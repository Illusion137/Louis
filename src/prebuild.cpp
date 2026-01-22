#include "frozen/string.h"
#include "gen/courseid_attribute_map.hpp"
#include "utils/cache.hpp"
#include "utils/str.hpp"
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <ranges>
#include <algorithm>
#include <format>
#include <fstream>
#include <vector>
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

std::string auto_generated_header_line(){
    return std::format("/* --AUTO-GENERATED-- by {} */\n", __FILE_NAME__);
}

void generate_enum_hpp(const char *filename, LLEnum& llenum) noexcept{
    std::string filepath = std::string{GEN_DIRECTORY} + filename;
    std::ofstream outfile;
    outfile.open(filepath);

    if(!outfile.is_open()) LL_LOG_FATAL("Failed to open file {} to generate enum {}: {}", filepath , llenum.name, filename);
    
    llenum.values.insert(llenum.values.begin(), {.id="UNKNOWN", .text="UNKNOWN"});
    
    outfile << auto_generated_header_line();
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

    auto filtered_courses = courses | std::views::filter([](auto course) { return !course.Subject.contains("@"); });

    outfile << auto_generated_header_line();
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
    if(argc >= 2 && strcmp(argv[1], "--clear") == 0) {
        cache::clear_cache();
    }

    const auto campuses = napi::connect::get_campuses();
    LL_IF_LOG_FATAL(campuses.size() == 0, "Campuses must have entries");
    LLEnum campus_enum = {
        .name = "Campus",
        .eclass = true, 
        .values = emap(campuses, [](auto campus) -> LLEnum::LLEnum_Value { 
            return {.id = campus.Value, .text = campus.Text}; 
        })
    };
    generate_enum_hpp("campus.hpp", campus_enum);

    const auto catalog_years = napi::transfer_plan::get_catalog_years();
    LL_IF_LOG_FATAL(catalog_years.size() == 0, "Catalog Years must have entries");
    LLEnum catalog_years_enum = {
        .name = "CatalogYears",
        .eclass = true,
        .values = emap(catalog_years, [](auto catalog_year) -> LLEnum::LLEnum_Value { 
            return {.id = "Y" + catalog_year.Value, .text = catalog_year.Text}; 
        })
    };
    generate_enum_hpp("catalog_years.hpp", catalog_years_enum);

    const auto attribute_courses = napi::edu::get_course_attribute_courses((CatalogTerms)1261);
    LL_IF_LOG_FATAL(attribute_courses.size() == 0, "Atrribute-Courses must have entries");
    std::unordered_map<std::string, uint64_t> course_attribute_map{};
    for(const auto &course: attribute_courses){
        std::println("COURSE: {} {}", course.subject, course.catnum);

        uint64_t all_course_attributes = CourseAttributes::COURSE_ATTRIBUTE_NONE;
        const auto attributes = split_string(course.crseattr, "|");
        for(const auto &attribute: attributes){
            const uint64_t course_attribute = courseattribute_fstrmap.at(frozen::string{attribute});
            all_course_attributes |= course_attribute;
        }
        course_attribute_map.insert({course.subject + course.catnum, all_course_attributes});
    }
    
    const auto course_catalog = napi::course_catalog::get_course_catalog();
    LL_IF_LOG_FATAL(course_catalog.size() == 0, "Course Catalog must have entries");

    auto course_catalog_subjects = emap(course_catalog, [](auto course) { return course.Subject; })
        | std::views::filter([](std::string nbr) { return !nbr.contains("@"); });
    std::unordered_set<std::string> course_catalog_subject_set(course_catalog_subjects.begin(), course_catalog_subjects.end());
    LLEnum course_subject_enum = {
        .name = "CourseSubject",
        .eclass = true,
        .values = emap(course_catalog_subject_set, [](auto subject) -> LLEnum::LLEnum_Value {
            return {.id = subject, .text = subject}; 
        })
    };

    std::vector<std::string> course_catalog_nbrs = emap(course_catalog, [](auto course) { return course.CatalogNbr; });
    std::unordered_set<std::string> course_catalog_nbrs_set(course_catalog_nbrs.begin(), course_catalog_nbrs.end());
    LLEnum course_nbr_enum = {
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

// #include <iostream>
// #include <gen/course_subjects.hpp>
// #include <gen/course_nbrs.hpp>
// #include <napi/napi_prereq.hpp>

// int main3(int argc, char **argv) {
//     std::ios::sync_with_stdio(false);
//     std::cin.tie(nullptr);

//     std::string path = "/Users/illusion/dev/Louis/temp/pre2.txt";
//     if (argc >= 2) path = argv[1];

//     // Replace demo maps with your frozen maps here:
//     const auto &subject_map = coursesubject_fstrmap;
//     const auto &nbr_map = coursenbr_fstrmap;

//     std::vector<std::string> lines = read_lines_split_entries(path);
//     if (lines.empty()) {
//         std::cerr << "No lines loaded from " << path << "\n";
//         return 2;
//     }
//     ParseStorage storage;
//     storage.records.reserve(lines.size());

//     // parse sequentially (fast). For further speed, parallelize across lines.
//     for (size_t i = 0; i < lines.size(); ++i) {
//         parseLineAndStore(storage, subject_map, nbr_map, lines[i]);
//     }

//     // Print small summary
//     std::cerr << "Parsed " << storage.records.size() << " entries\n";
//     std::cerr << "Nodes: " << storage.nodes.size() << "  Children total: " << storage.children.size() << "  Unique requirements: " << storage.req_pool.size() << "\n";

//     for(uint32_t i = 0; i < storage.nodes.size(); i++) {
//         if(!coursesubject_tstrmap.contains(storage.nodes[i].subject)) continue;
//         std::string subject = std::string{coursesubject_tstrmap.at(storage.nodes[i].subject).data()};
//         std::string nbr = std::string{coursenbr_tstrmap.at(storage.nodes[i].number).data()};
//         LL_INFO("{}: {} {}", i, subject, nbr);
//     }

//     for (size_t i = 0; i < storage.records.size(); ++i) {
//         auto &r = storage.records[i];
//         std::cout << "ENTRY[" << i << "]: raw=\""<< r.raw << "\"" << "\n"
//              << " prereq_node=" << (r.prereq_node_index==UINT32_MAX?std::string("none"):std::to_string(r.prereq_node_index)) << "\n"
//              << " coreq_node=" << (r.coreq_node_index==UINT32_MAX?std::string("none"):std::to_string(r.coreq_node_index)) << "\n"
//              << " honors=" << (r.honors? "Y":"N") << "\n"
//              << " foundationEnglish=" << (r.foundationEnglish? "Y":"N") << "\n"
//              << " intlExch=" << (r.internationalExchange? "Y":"N") << "\n"
//              << " minCr=" << r.minCredits << "\n"
//              << " gpa=" << r.gpa << "\n" 
//              << "\n\n";
//         if(r.prereq_node_index==UINT32_MAX) continue;
//         if(!coursesubject_tstrmap.contains(storage.nodes[r.prereq_node_index].subject)) {
//             for(int j = 0; j < storage.nodes[r.prereq_node_index].child_count; j++)
//             {
//                 LL_INFO("{}", (int)storage.nodes[j + storage.nodes[r.prereq_node_index].child_pos].type);
//                 if(storage.nodes[j + storage.nodes[r.prereq_node_index].child_pos].type == NodeType::Course){
//                     std::string subject = std::string{coursesubject_tstrmap.at(storage.nodes[j + storage.nodes[r.prereq_node_index].child_pos].subject).data()};
//                     std::string nbr = std::string{coursenbr_tstrmap.at(storage.nodes[j + storage.nodes[r.prereq_node_index].child_pos].number).data()};
//                     LL_INFO("{}: {} {}", i, subject, nbr);
//                 }
//             }
//             continue;
//         }
//         std::string subject = std::string{coursesubject_tstrmap.at(storage.nodes[r.prereq_node_index].subject).data()};
//         std::string nbr = std::string{coursenbr_tstrmap.at(storage.nodes[r.prereq_node_index].number).data()};
//         LL_INFO("{}: {} {}", i, subject, nbr);
//     }

//     // At this point, storage.nodes, storage.children, storage.req_pool, storage.records
//     // contain everything you need to emit a constexpr header.
    
//     // If you want, I can add code to emit a .hpp directly that defines:
//     // - constexpr char req_strings[][N] = {...}
//     // - constexpr Node nodes[] = { ... }  (with Node layout adapted to constexpr)
//     // - constexpr uint32_t children[] = { ... }
//     // - constexpr ParsedRequisite records[] = { ... }
    
//     // Tell me if you want the header emitter next.

//     return 0;
// }