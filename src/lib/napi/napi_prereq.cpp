#include <iostream>
#include <fstream>
#include <sstream>
#include "napi_prereq.hpp"

// ---------------------------
// Fast helpers
// ---------------------------
std::string uppercase_copy(std::string_view sv) {
    std::string s; s.reserve(sv.size());
    for (char c : sv) s.push_back((char)toupper((unsigned char)c));
    return s;
}

void trim_inplace(std::string &s) {
    size_t a = 0;
    while (a < s.size() && isspace((unsigned char)s[a])) ++a;
    size_t b = s.size();
    while (b > a && isspace((unsigned char)s[b-1])) --b;
    if (a || b != s.size()) s = s.substr(a, b-a);
}
std::string_view trim_view(std::string_view sv) {
    size_t a = 0;
    while (a < sv.size() && isspace((unsigned char)sv[a])) ++a;
    size_t b = sv.size();
    while (b > a && isspace((unsigned char)sv[b-1])) --b;
    return sv.substr(a, b-a);
}

// fast integer parse (reads leading digits, returns value and move index)
uint32_t parse_uint32_from_sv(std::string_view sv, size_t &idx) {
    uint32_t v = 0;
    while (idx < sv.size() && isdigit((unsigned char)sv[idx])) {
        v = v*10 + (sv[idx]-'0');
        ++idx;
    }
    return v;
}

// ---------------------------
// Course extraction using provided maps
// - tries "SUBJ NBR" then "SUBJNBR" merged
// - returns true if resolved to enums
// ---------------------------
bool extractCourseToken(const SubjectMap &subjmap, const NBRMap &nbrmap,
                        std::string_view token, CourseSubject &out_subj, CourseNBR &out_nbr)
{
    // normalize separators: replace '-' with ' '
    std::string tmp; tmp.reserve(token.size());
    for (char c : token) tmp.push_back(c == '-' ? ' ' : c);
    // split into alnum runs
    std::vector<std::string_view> parts;
    size_t i = 0;
    while (i < tmp.size()) {
        // skip non-alnum
        while (i < tmp.size() && !isalnum((unsigned char)tmp[i])) ++i;
        if (i >= tmp.size()) break;
        size_t j = i;
        while (j < tmp.size() && isalnum((unsigned char)tmp[j])) ++j;
        parts.emplace_back(tmp.data()+i, j-i);
        i = j;
    }

    // CASE A: parts[0]=subject letters, parts[1]=number
    if (parts.size() >= 2) {
        std::string subjU = uppercase_copy(parts[0]);
        auto sit = subjmap.find(frozen::string{subjU});
        if (sit != subjmap.end()) {
            // second part might be number+suffix (we attempt to match exact nbr string)
            std::string nbrStr; nbrStr.reserve(parts[1].size());
            for (char c : parts[1]) nbrStr.push_back((char)toupper((unsigned char)c));
            auto nit = nbrmap.find(frozen::string{nbrStr});
            if (nit != nbrmap.end()) {
                out_subj = sit->second;
                out_nbr = nit->second;
                return true;
            }
        }
    }

    // CASE B: merged token(s) where letters + digits in same part e.g. "CS122" or "BIO182H"
    for (auto &pview : parts) {
        // find first non-alpha
        size_t pos = 0;
        while (pos < pview.size() && isalpha((unsigned char)pview[pos])) ++pos;
        if (pos == 0 || pos >= pview.size()) continue;
        std::string subjU = uppercase_copy(pview.substr(0, pos));
        auto sit = subjmap.find(frozen::string{subjU});
        if (sit == subjmap.end()) continue;
        // remaining is number (possibly with letter suffix)
        std::string nbrStr; nbrStr.reserve(pview.size()-pos);
        for (char c : pview.substr(pos)) nbrStr.push_back((char)toupper((unsigned char)c));
        auto nit = nbrmap.find(frozen::string{nbrStr});
        if (nit != nbrmap.end()) {
            out_subj = sit->second;
            out_nbr = nit->second;
            return true;
        }
    }

    return false;
}

// ---------------------------
// splitTopLevel: split expr by top-level operator at parentheses depth 0
// operator must be a lowercase ascii word, e.g. "and" or "or"
// returns vector of string_views (referencing original expr)
std::vector<std::string_view> splitTopLevel(std::string_view text, std::string_view op) {
    std::vector<std::string_view> out;
    size_t n = text.size();
    int depth = 0;
    size_t segStart = 0;
    for (size_t i = 0; i < n; ++i) {
        char c = text[i];
        if (c == '(') { ++depth; continue; }
        if (c == ')') { --depth; continue; }
        if (depth == 0) {
            // check possible op match (case-insensitive) with word boundaries
            if (i + op.size() <= n) {
                bool match = true;
                for (size_t k = 0; k < op.size(); ++k) {
                    if (tolower((unsigned char)text[i+k]) != op[k]) { match = false; break; }
                }
                if (match) {
                    // check left & right boundaries (not alpha)
                    bool left_ok = (i == 0) || !isalpha((unsigned char)text[i-1]);
                    size_t j = i + op.size();
                    bool right_ok = (j >= n) || !isalpha((unsigned char)text[j]);
                    if (left_ok && right_ok) {
                        // push segment
                        out.emplace_back(text.data() + segStart, i - segStart);
                        // advance i to after op; skip spaces
                        i = j;
                        while (i < n && isspace((unsigned char)text[i])) ++i;
                        segStart = i;
                        --i; // for loop ++i will correct
                    }
                }
            }
        }
    }
    // final segment
    if (segStart < n) {
        out.emplace_back(text.data() + segStart, n - segStart);
    }
    // trim segments
    for (auto &s : out) {
        // trim edges
        size_t a = 0;
        while (a < s.size() && isspace((unsigned char)s[a])) ++a;
        size_t b = s.size();
        while (b > a && isspace((unsigned char)s[b-1])) --b;
        s = s.substr(a, b-a);
    }
    return out;
}

// ---------------------------
// parseLogic returns index of node added into storage.nodes
// ---------------------------
uint32_t parseLogic(ParseStorage &store,
                    const SubjectMap &subjmap,
                    const NBRMap &nbrmap,
                    std::string_view expr);

// helper to create and push Node and return its index
uint32_t push_node(ParseStorage &store, const Node &n) {
    uint32_t idx = (uint32_t)store.nodes.size();
    store.nodes.push_back(n);
    return idx;
}

// add requirement string to pool & return index
uint32_t add_req(ParseStorage &store, std::string_view text) {
    std::string s(text);
    trim_inplace(s);
    if (s.empty()) return UINT32_MAX;
    auto it = store.req_index_map.find(s);
    if (it != store.req_index_map.end()) return it->second;
    uint32_t idx = (uint32_t)store.req_pool.size();
    store.req_pool.push_back(s);
    store.req_index_map.emplace(store.req_pool.back(), idx);
    return idx;
}

uint32_t parseLogic_impl(ParseStorage &store,
                         const SubjectMap &subjmap,
                         const NBRMap &nbrmap,
                         std::string_view expr)
{
    expr = trim_view(expr);
    if (expr.empty()) {
        return UINT32_MAX;
    }

    // strip outer parentheses if they wrap entire expression
    if (expr.front() == '(' && expr.back() == ')') {
        int depth = 0; bool encloses = true;
        for (size_t i = 0; i < expr.size(); ++i) {
            if (expr[i] == '(') ++depth;
            else if (expr[i] == ')') {
                --depth;
                if (depth == 0 && i+1 < expr.size()) { encloses = false; break; }
            }
        }
        if (encloses) {
            return parseLogic_impl(store, subjmap, nbrmap, expr.substr(1, expr.size()-2));
        }
    }

    // try OR split
    {
        auto parts = splitTopLevel(expr, "or");
        if (parts.size() > 1) {
            // parse children
            std::vector<uint32_t> child_indices;
            child_indices.reserve(parts.size());
            for (auto &p : parts) {
                uint32_t ci = parseLogic_impl(store, subjmap, nbrmap, p);
                if (ci != UINT32_MAX) child_indices.push_back(ci);
            }
            // create node
            Node n;
            n.type = NodeType::Or;
            n.child_pos = (uint32_t)store.children.size();
            n.child_count = (uint16_t)child_indices.size();
            for (auto c : child_indices) store.children.push_back(c);
            return push_node(store, n);
        }
    }

    // try AND split
    {
        auto parts = splitTopLevel(expr, "and");
        if (parts.size() > 1) {
            std::vector<uint32_t> child_indices;
            child_indices.reserve(parts.size());
            for (auto &p : parts) {
                uint32_t ci = parseLogic_impl(store, subjmap, nbrmap, p);
                if (ci != UINT32_MAX) child_indices.push_back(ci);
            }
            Node n;
            n.type = NodeType::And;
            n.child_pos = (uint32_t)store.children.size();
            n.child_count = (uint16_t)child_indices.size();
            for (auto c : child_indices) store.children.push_back(c);
            return push_node(store, n);
        }
    }

    // handle comma-separated lists outside parentheses as AND
    {
        std::vector<std::string_view> commaParts;
        // std::string_view cur;
        size_t depth = 0;
        size_t start = 0;
        for (size_t i = 0; i < expr.size(); ++i) {
            char ch = expr[i];
            if (ch == '(') ++depth;
            else if (ch == ')') --depth;
            if (ch == ',' && depth == 0) {
                commaParts.emplace_back(expr.data()+start, i-start);
                start = i+1;
            }
        }
        if (start < expr.size()) commaParts.emplace_back(expr.data()+start, expr.size()-start);
        if (commaParts.size() > 1) {
            std::vector<uint32_t> childrenIdx;
            for (auto p : commaParts) {
                uint32_t ci = parseLogic_impl(store, subjmap, nbrmap, p);
                if (ci != UINT32_MAX) childrenIdx.push_back(ci);
            }
            Node n;
            n.type = NodeType::And;
            n.child_pos = (uint32_t)store.children.size();
            n.child_count = (uint16_t)childrenIdx.size();
            for (auto c : childrenIdx) store.children.push_back(c);
            return push_node(store, n);
        }
    }

    // primitive token: try parse course
    {
        CourseSubject cs = CourseSubject::UNKNOWN;
        CourseNBR cn = CourseNBR::UNKNOWN;
        if (extractCourseToken(subjmap, nbrmap, expr, cs, cn)) {
            Node n;
            n.type = NodeType::Course;
            n.child_pos = 0;
            n.child_count = 0;
            n.subject = cs;
            n.number = cn;
            n.req_str_index = UINT32_MAX;
            return push_node(store, n);
        }
    }

    // fallback: requirement node
    {
        uint32_t req_idx = add_req(store, expr);
        Node n;
        n.type = NodeType::Requirement;
        n.child_pos = 0;
        n.child_count = 0;
        n.req_str_index = req_idx;
        n.subject = CourseSubject::UNKNOWN;
        n.number = CourseNBR::UNKNOWN;
        return push_node(store, n);
    }
}

uint32_t parseLogic(ParseStorage &store,
                    const SubjectMap &subjmap,
                    const NBRMap &nbrmap,
                    std::string_view expr)
{
    return parseLogic_impl(store, subjmap, nbrmap, expr);
}

// ---------------------------
// parseLine: extracts prereq/coreq portions and flags
// ---------------------------
void parseLineAndStore(ParseStorage &store,
                       const SubjectMap &subjmap,
                       const NBRMap &nbrmap,
                       const std::string &line)
{
    ParsedRequisite rec;
    rec.raw = line;
    std::string lower = line; transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return (char)tolower(c); });

    // find positions of "prerequisite" and "corequisite" (case-insensitive)
    size_t ppos = lower.find("prerequisite");
    size_t cpos = lower.find("corequisite");
    // size_t pos_start = std::string::npos, pos_end = std::string::npos;

    std::string_view prereq_sv, coreq_sv;

    if (ppos != std::string::npos) {
        // find colon after keyword if any
        size_t colon = line.find(':', ppos);
        size_t start = (colon == std::string::npos) ? ppos + strlen("prerequisite") : colon + 1;
        // end at coreq if coreq occurs after prerequisite
        size_t end = (cpos != std::string::npos && cpos > ppos) ? cpos : line.size();
        if (start < end) prereq_sv = trim_view(std::string_view(line.c_str()+start, end-start));
    }
    if (cpos != std::string::npos) {
        size_t colon = line.find(':', cpos);
        size_t start = (colon == std::string::npos) ? cpos + strlen("corequisite") : colon + 1;
        if (start < line.size()) coreq_sv = trim_view(std::string_view(line.c_str()+start));
    }

    // Some lines use "Pre- or Corequisite" or "Pre- or Corequisite:"
    if (prereq_sv.empty()) {
        size_t preorcpos = lower.find("pre- or corequisite");
        if (preorcpos == std::string::npos) preorcpos = lower.find("pre or corequisite");
        if (preorcpos != std::string::npos) {
            size_t colon = line.find(':', preorcpos);
            size_t start = (colon == std::string::npos) ? preorcpos + strlen("pre- or corequisite") : colon + 1;
            if (start < line.size()) prereq_sv = trim_view(std::string_view(line.c_str()+start));
        }
    }

    // parse prereq/coreq
    if (!prereq_sv.empty()) {
        uint32_t idx = parseLogic(store, subjmap, nbrmap, prereq_sv);
        if (idx != UINT32_MAX) rec.prereq_node_index = idx;
    }
    if (!coreq_sv.empty()) {
        uint32_t idx = parseLogic(store, subjmap, nbrmap, coreq_sv);
        if (idx != UINT32_MAX) rec.coreq_node_index = idx;
    }

    // flags: honors, foundation english, international exchange
    if (lower.find("honors student") != std::string::npos || lower.find("honors group") != std::string::npos)
        rec.honors = true;
    if (lower.find("foundation english") != std::string::npos || lower.find("foundation english requirement") != std::string::npos)
        rec.foundationEnglish = true;
    if (lower.find("international exchange") != std::string::npos)
        rec.internationalExchange = true;

    // extract first integer units/credits (approx)
    // search for " units" or " credits" near digits
    for (size_t i = 0; i+5 < line.size(); ++i) {
        if (isdigit((unsigned char)line[i])) {
            // read integer
            size_t j = i;
            uint32_t v = 0;
            while (j < line.size() && isdigit((unsigned char)line[j])) { v = v*10 + (line[j]-'0'); ++j; }
            // look ahead for "unit" or "credit" within small window
            size_t k = j;
            while (k < line.size() && isspace((unsigned char)line[k])) ++k;
            if (k+3 < line.size()) {
                std::string_view nxt(line.c_str()+k, std::min<size_t>(12, line.size()-k));
                std::string tmp = uppercase_copy(nxt);
                if (tmp.find("UNIT") != std::string::npos || tmp.find("CREDIT") != std::string::npos || tmp.find("EARNED HOUR") != std::string::npos) {
                    rec.minCredits = (uint16_t)std::min<uint32_t>(v, UINT16_MAX);
                    break;
                }
            }
            i = j;
        }
    }

    // extract GPA pattern "GPA of 3.0" or "overall GPA of 3.0"
    for (size_t i = 0; i+3 < lower.size(); ++i) {
        if (lower[i] == 'g' && lower[i+1]=='p' && lower[i+2]=='a') {
            // skip "gpa", then optional "of" and spaces, then digits
            size_t j = i+3;
            while (j < lower.size() && (lower[j]==' ' || lower[j]==':' )) ++j;
            if (j+1 < lower.size() && lower[j]=='o' && lower[j+1]=='f') ++j;
            while (j < lower.size() && isspace((unsigned char)lower[j])) ++j;
            // read float like d.d
            if (j < lower.size() && isdigit((unsigned char)lower[j])) {
                size_t k = j;
                int intpart = 0;
                while (k < lower.size() && isdigit((unsigned char)lower[k])) { intpart = intpart*10 + (lower[k]-'0'); ++k; }
                float val = (float)intpart;
                if (k < lower.size() && lower[k]=='.') {
                    ++k;
                    if (k < lower.size() && isdigit((unsigned char)lower[k])) {
                        val += (lower[k]-'0') / 10.0f;
                    }
                }
                rec.gpa = val;
                break;
            }
        }
    }

    store.records.push_back(std::move(rec));
}

// ---------------------------
// File loading helpers
// ---------------------------
std::vector<std::string> read_lines_split_entries(const std::string &path) {
    std::vector<std::string> out;
    std::ifstream ifs(path);
    if (!ifs) {
        std::cerr << "Failed to open " << path << "\n";
        return out;
    }
    // read entire file into string
    std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    // Heuristic: if file is a JSON array of quoted strings or a large single line with many entries
    if (data.size() > 0 && (data.front() == '[' || data.find("\",\"") != std::string::npos)) {
        // split on "," occurrences that look like separators between quoted strings
        // simple finite-state machine: inquote?
        std::string cur;
        cur.reserve(256);
        bool inquote = false;
        for (size_t i = 0; i < data.size(); ++i) {
            char c = data[i];
            if (c == '"') {
                inquote = !inquote;
                continue; // drop quotes
            }
            if (!inquote && c == ',' ) {
                // end of entry
                std::string s = cur;
                trim_inplace(s);
                if (!s.empty()) out.push_back(s);
                cur.clear();
                continue;
            }
            if (inquote || (c != '\n' && c != '\r')) cur.push_back(c);
            if (!inquote && (c == '\n' || c == '\r')) {
                std::string s = cur;
                trim_inplace(s);
                if (!s.empty()) out.push_back(s);
                cur.clear();
            }
        }
        if (!cur.empty()) {
            std::string s = cur;
            trim_inplace(s);
            if (!s.empty()) out.push_back(s);
        }
    } else {
        // simple line-by-line
        std::istringstream iss(data);
        std::string line;
        while (getline(iss, line)) {
            trim_inplace(line);
            if (!line.empty()) out.push_back(line);
        }
    }
    return out;
}