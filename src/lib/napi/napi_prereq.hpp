#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <sll.hpp>
#include <gen/course_subjects.hpp>
#include <gen/course_nbrs.hpp>

typedef frozen::unordered_map<frozen::string, CourseSubject, 189> SubjectMap;
typedef frozen::unordered_map<frozen::string, CourseNBR, 1192> NBRMap;

enum class NodeType : uint8_t { 
    Course = 0,
    Requirement = 1,
    And = 2,
    Or = 3
};

// Compact node representation:
// - children are stored as contiguous indices in children vector [child_pos, child_pos+child_count)
// - requirement strings indexed into req_pool
struct Node {
    NodeType type;
    // children slice:
    uint32_t child_pos;
    uint16_t child_count;
    // course (valid when type==Course)
    CourseSubject subject;
    CourseNBR number;
    // requirement (valid when type==Requirement)
    uint32_t req_str_index; // index into req_pool
    Node() : type(NodeType::Requirement), child_pos(0), child_count(0),
             subject(CourseSubject::UNKNOWN), number(CourseNBR::UNKNOWN), req_str_index(UINT32_MAX) {}
};

// parsed line record
struct ParsedRequisite {
    uint32_t prereq_node_index; // index into nodes or UINT32_MAX if none
    uint32_t coreq_node_index;  // index into nodes or UINT32_MAX if none
    bool honors;
    bool foundationEnglish;
    bool internationalExchange;
    uint16_t minCredits;
    uint16_t maxCredits;
    float gpa;
    std::string raw;
    ParsedRequisite() : prereq_node_index(UINT32_MAX), coreq_node_index(UINT32_MAX),
                       honors(false), foundationEnglish(false), internationalExchange(false),
                       minCredits(0), maxCredits(0), gpa(0.0f) {}
};

// global storage for a single run (we return or serialize these)
struct ParseStorage {
    std::vector<Node> nodes;
    std::vector<uint32_t> children; // child node indices
    std::vector<std::string> req_pool; // unique requirement strings
    std::unordered_map<std::string, uint32_t> req_index_map; // dedupe
    std::vector<ParsedRequisite> records;
    ParseStorage() { nodes.reserve(1<<16); children.reserve(1<<16); req_pool.reserve(1<<12); }
};

// ---------------------------
// Fast helpers
// ---------------------------
LL_API std::string uppercase_copy(std::string_view sv);

LL_API void trim_inplace(std::string &s);
LL_API std::string_view trim_view(std::string_view sv);

// fast integer parse (reads leading digits, returns value and move index)
LL_API uint32_t parse_uint32_from_sv(std::string_view sv, size_t &idx);

// ---------------------------
// Course extraction using provided maps
// - tries "SUBJ NBR" then "SUBJNBR" merged
// - returns true if resolved to enums
// ---------------------------
LL_API bool extractCourseToken(const SubjectMap &subjmap, const NBRMap &nbrmap,
                        std::string_view token, CourseSubject &out_subj, CourseNBR &out_nbr);

// ---------------------------
// splitTopLevel: split expr by top-level operator at parentheses depth 0
// operator must be a lowercase ascii word, e.g. "and" or "or"
// returns vector of string_views (referencing original expr)
LL_API std::vector<std::string_view> splitTopLevel(std::string_view text, std::string_view op);

// ---------------------------
// parseLogic returns index of node added into storage.nodes
// ---------------------------
LL_API uint32_t parseLogic(ParseStorage &store,
                    const SubjectMap &subjmap,
                    const NBRMap &nbrmap,
                    std::string_view expr);

// helper to create and push Node and return its index
LL_API uint32_t push_node(ParseStorage &store, const Node &n);

// add requirement string to pool & return index
LL_API uint32_t add_req(ParseStorage &store, std::string_view text);

LL_API uint32_t parseLogic_impl(ParseStorage &store,
                         const SubjectMap &subjmap,
                         const NBRMap &nbrmap,
                         std::string_view expr);

LL_API uint32_t parseLogic(ParseStorage &store,
                    const SubjectMap &subjmap,
                    const NBRMap &nbrmap,
                    std::string_view expr);

// ---------------------------
// parseLine: extracts prereq/coreq portions and flags
// ---------------------------
LL_API void parseLineAndStore(ParseStorage &store,
                       const SubjectMap &subjmap,
                       const NBRMap &nbrmap,
                       const std::string &line);

// ---------------------------
// File loading helpers
// ---------------------------
LL_API std::vector<std::string> read_lines_split_entries(const std::string &path);