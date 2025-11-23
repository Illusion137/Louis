
// #include <vector>
// #include <array>
// #include <cstdint>

#include <cstdint>
#include <napi/napi.hpp>

// struct Time {
//     uint_fast8_t hours;
//     uint_fast8_t minutes;
//     Time operator+(const Time &other){
//         // TODO assume no 11:00pm - 2:00am for now
//         return Time{static_cast<uint_fast8_t>(other.hours + this->hours)};
//     }
// };

// struct Date {
//     uint16_t year;
//     uint8_t month;
//     uint8_t day;
// };

// enum class Instructor {

// };
// enum class ClassSectionStatus {
//     OPEN,
//     CLOSED,
//     WAITLIST
// };

// enum class ClassSubjectPrefix {
//     ACC,
//     ADV,
//     AHB,
//     AIS,
//     ANT,
//     APMS,
//     ARE,
//     ARH,
//     ART,
//     AST,
//     AT,
//     BA,
//     BAN,
//     BBA,
//     BIO,
//     BME,
//     BSCI,
//     BUS,
//     CAR,
//     CCHE,
//     CCJ,
//     CCS,
//     CENE,
//     CHM,
//     CIE,
//     CLA,
//     CM,
//     CMF,
//     COM,
//     CS,
//     CSD,
//     CST,
//     CTE,
//     CYB,
//     DH,
//     DIS,
//     ECI,
//     ECO,
//     EDF,
//     EDL,
//     EDR,
//     EE,
//     EES,
//     ENG,
//     ENV,
//     EPS,
//     ES,
//     ESE,
//     ETC,
//     FIN,
//     FOR,
//     FRE,
//     FW,
//     FYS,
//     GLG,
//     GSP,
//     HA,
//     HIS,
//     HON,
//     HS,
//     HUM,
//     IH,
//     INF,
//     ISM,
//     ITC,
//     JPN,
//     JUS,
//     LAN,
//     LAS,
//     LEA,
//     MAT,
//     ME,
//     MGBA,
//     MGT,
//     MKT,
//     MOL,
//     MUS,
//     NAUY,
//     NTS,
//     NUR,
//     OTD,
//     PADM,
//     PE,
//     PHA,
//     PHI,
//     PHO,
//     PHS,
//     PHY,
//     PM,
//     POS,
//     PR,
//     PRM,
//     PSY,
//     PT,
//     REL,
//     SA,
//     SCI,
//     SOC,
//     SPA,
//     STA,
//     STR,
//     SUS,
//     SW,
//     TH,
//     TSM,
//     WGS
// };

// struct ClassSectionTime {
//     uint16_t start;
//     uint16_t end;
//     inline bool overlaps(const ClassSectionTime &other_time) const noexcept{
//         return this->end < other_time.start || other_time.end < this->start;
//     }
// };

// class ClassSchedule {
// public:
//     bool add_class(const ClassScheduledSection *const section, const bool allow_waitlist) noexcept{
//         if(scheduled_section_length >= scheduled_sections.max_size()) return false;
//         if(section->status == ClassSectionStatus::CLOSED) return false;
//         if(section->status == ClassSectionStatus::WAITLIST && !allow_waitlist) return false;
//         for(int i = 0; i < scheduled_section_length; i++) {
//             if(!section->time.overlaps(scheduled_sections[i]->time)){
//                 scheduled_sections[scheduled_section_length] = section;
//                 scheduled_section_length++;
//                 return true;
//             }
//         }
//         return false;
//     }
//     void force_add_class(const ClassScheduledSection *const section) noexcept{
//         scheduled_sections[scheduled_section_length] = section;
//         scheduled_section_length++;
//     }
// private:
//     uint16_t scheduled_section_length;
//     std::array<const ClassScheduledSection*, 16> scheduled_sections;
// };

// struct ClassNodeAdjacencyMap {
    
// };

int main(int argc, char **argv){
    // std::vector<ClassScheduledSection*> all_class_scheduled_sections;
    // std::vector<std::vector<ClassScheduledSection*>> scheduled_sections_adjacency_map;
    
    // scheduled_sections_adjacency_map.reserve(all_class_scheduled_sections.size());
    
    // for(int i = 0; i < all_class_scheduled_sections.size(); i++){
    //     scheduled_sections_adjacency_map[i].reserve(all_class_scheduled_sections.size());
    // }

    // cpr::Response napi_api_docs_response = cpr::Get(cpr::Url{"https://api.extended.nau.edu/Help"});
    // const std::string &napi_api_docs_html = napi_api_docs_response.text;
    

    // std::println("Status: {}, Content-Type: {}, Txt: {}", r.status_code , r.header["content-type"],r.text);

    const auto bar = napi::connect::get_campuses();
    for(uint32_t i = 0; i < bar.size(); i++) {
        std::println("{}", bar[i].Text);
    }

    return 0;
}