#include <gen/course_subjects.hpp>
#include <cstdint>
#include <glaze/core/opts.hpp>
#include <vector>
#include <glaze/glaze.hpp>
#include <cpr/cpr.h>
#include <sll.hpp>
#include <rozfetch.hpp>
#include <gen/catalog_terms.hpp>
// #include <gen/courses.hpp>
// #include <gen/campus.hpp>
// #include <rui/distance.hpp>

#define DAYS_IN_WEEK 7

#define NAPI_EXTENDED_BASE_URL "https://api.extended.nau.edu/api/"
#define NAPI_EDU_BASE_URL "https://catalog.nau.edu/"
#define NAPI_PEOPLESOFT_BASE_URL "https://www.peoplesoft.nau.edu/"

namespace napi {
    namespace connect {
        struct Campuses {
            struct Campus {
                std::string Value;
                std::string Text;
            };
            std::vector<Campus> campus;
        };
        LL_API std::vector<Campuses::Campus> get_campuses();
    };
    namespace transfer_plan {
        //https://api.extended.nau.edu/api/TransferPlan/GetCatalogYears
        struct CatalogYears {
            struct CatalogYears_catalogYears {
                std::string Value;
                std::string Text;
            };
            std::vector<CatalogYears_catalogYears> catalogYears;
        };
        LL_API std::vector<CatalogYears::CatalogYears_catalogYears> get_catalog_years();
    };
    namespace course_catalog {
        struct CourseCatalogItem {
            std::string Subject;
            std::string CatalogNbr;
            std::string Title;
            std::string LongTitle;
            float Credits;
            std::string Level;
            std::string Prerequisites;
            std::string Description;
            std::string Materials;
        };
        typedef std::vector<CourseCatalogItem> CourseCatalog;
        LL_API CourseCatalog get_course_catalog();
    };
    namespace edu {
        struct DropdownOption {
            std::string text;
            std::string value;
        };
        struct CourseResult {
            std::string catnbrcategory;
            std::string crseattrattrval;
            std::string crseattr;
            std::string crseattrdescr;
            std::string crseattrvalue;
            std::string crseattrvaluedescr;
            std::string subject;
            std::string catnum;
            std::string suncrse;
            std::string href;
        };
        struct CourseDropdowns {
            std::vector<DropdownOption> catalog_terms_dropdown;
            std::vector<DropdownOption> course_attributes_dropdown;
            std::vector<DropdownOption> course_attributes_values_dropdown;
        };
        LL_API CourseDropdowns get_catalog_dropdowns();
        LL_API std::vector<CourseResult> get_course_attribute_courses(const CatalogTerms term);
    };
    namespace peoplesoft {
        struct AcademicAdvisorReport_ProgramInformation {};
        struct AcademicAdvisorReport_CurrentAcademicSummary {};
        struct AcademicAdvisorReport_UniversityRequirements {};
        struct AcademicAdvisorReport_InclusivePerspectives {};
        struct AcademicAdvisorReport_GeneralStudies {};
        struct AcademicAdvisorReport_GeneralStudiesRequirements {};
        struct AcademicAdvisorReport_MajorRequirements {};
        struct AcademicAdvisorReport_MinorRequirements {};
        struct AcademicAdvisorReport_Electives {};
        struct AcademicAdvisorReport {
            std::string name; 
            uint_fast16_t nau_id;
        };

        struct Date {
            uint16_t year;
            uint8_t month;
            uint8_t day;
        };
        struct ClassSectionTime {
            uint_fast16_t start_min;
            uint_fast16_t end_min;
            inline bool overlaps(const ClassSectionTime &other_time) const noexcept{
                return this->end_min < other_time.start_min || other_time.end_min < this->start_min;
            }
        };
        enum ClassSectionDays: uint8_t {
            CLASS_DAY_SUNDAY = 1,
            CLASS_DAY_MONDAY = 1 << 1,
            CLASS_DAY_TUESDAY = 1 << 2,
            CLASS_DAY_WEDNESDAY = 1 << 3,
            CLASS_DAY_THURSDAY = 1 << 4,
            CLASS_DAY_FRIDAY = 1 << 5,
            CLASS_DAY_SATURDAY = 1 << 6
        };
        enum class InstructionMode {
            IN_PERSON,
            BLENDED_LEARNING,
            NAU_FLEX,
            ONLINE
        };
        enum class ClassSectionStatus {
            OPEN,
            CLOSED,
            WAITLIST
        };
        struct SearchCoursesParams {
            uint32_t ICAJAX = 1;
            uint32_t ICNAVTYPEDROPDOWN = 0;
            std::string ICType = "Panel";
            uint32_t ICElementNum = 0;
            uint32_t ICStateNum = 4;
            std::string ICAction = "CLASS_SRCH_WRK2_SSR_PB_CLASS_SRCH";
            uint32_t ICModelCancel = 0;
            uint32_t ICXPos = 0;
            uint32_t ICYPos = 0;
            int32_t ResponsetoDiffFrame = -1;
            std::string TargetFrameName = "None";
            std::string FacetPath = "None";
            std::string PrmtTbl = "";
            std::string PrmtTbl_fn = "";
            std::string PrmtTbl_fv = "";
            std::string TA_SkipFldNms = "";
            std::string ICFocus = "";
            uint32_t ICSaveWarningFilter = 0;
            int32_t ICChanged = -1;
            uint32_t ICSkipPending = 0;
            uint32_t ICAutoSave = 0;
            uint32_t ICResubmit = 0;
            std::string ICSID = "pGqKLGPuNjoH4TB5GTQ%2FhsERaF10cDe%2Brm7iGirXmgw%3D";
            bool ICActionPrompt = false;
            std::string ICBcDomData = "UnknownValue";
            std::string ICPanelName = "";
            std::string ICFind = "";
            std::string ICAddCount = "";
            std::string ICAppClsData = "";
            std::string CLASS_SRCH_WRK2_INSTITUTION$31$ = "NAU00"; // institution // 
            uint32_t CLASS_SRCH_WRK2_STRM$35$ = 0; // catalog term // 1261
            std::string SSR_CLSRCH_WRK_SUBJECT_SRCH$0 = ""; // subject // CS
            std::string SSR_CLSRCH_WRK_SSR_EXACT_MATCH1$1 = "E"; // E
            std::string SSR_CLSRCH_WRK_CATALOG_NBR$1 = "";
            std::string SSR_CLSRCH_WRK_N__ONLY_ONLINE_CMP$chk$3 = "";
            std::string SSR_CLSRCH_WRK_CAMPUS$3 = "";
            std::string SSR_CLSRCH_WRK_CRSE_ATTR$6 = "";
            std::string SSR_CLSRCH_WRK_CRSE_ATTR_VALUE$6 = "";
            std::string SSR_CLSRCH_WRK_ACAD_CAREER$7 = "";
            std::string SSR_CLSRCH_WRK_SSR_OPEN_ONLY$chk$8 = "N"; // Y
            std::string SSR_CLSRCH_WRK_SSR_OPEN_ONLY$8 = "N"; // Y
        };
        static inline SearchCoursesParams create_search_course_params(CatalogTerms term, CourseSubject subject){
            SearchCoursesParams params;
            params.CLASS_SRCH_WRK2_STRM$35$ = (uint32_t)term;
            params.SSR_CLSRCH_WRK_SUBJECT_SRCH$0 = coursesubject_tstrmap.at(subject).data();
            return params;
        }
        // struct ClassScheduledSection {
        //     ClassNode *class_node;
        //     uint8_t section_id;
        //     uint16_t class_id; // this is unique to every class time
        //     ClassSectionDays days;
        //     ClassSectionTime time;
        //     Building room_building;
        //     uint16_t room_no;
        //     char room_no_suffix[4];
        //     // Instructor instructor;
        //     Date meeting_date_start;
        //     Date meeting_date_end;
        //     Campus campus;
        //     InstructionMode instruction_mode;
        //     uint16_t available_seats;
        //     bool reserved_seating;
        //     ClassSectionStatus status;
        //     // char sun_id[32];
        //     bool overlaps(const ClassScheduledSection &other_section) const noexcept{
        //         if(this->instruction_mode == InstructionMode::ONLINE) return false;
        //         if(other_section.instruction_mode == InstructionMode::ONLINE) return false;
        //         // TODO check meeting_date_start and meeting date end
        //             // two 8 weeks on opposite sides can't overlap
        //         const uint32_t shared_days = this->days & other_section.days;
        //         if(shared_days == 0) return false;
        //         return this->time.overlaps(other_section.time);
        //     }
        //     // NOTE: this doesn't check for overlap because it's assume to already be checked
        //     bool can_make_it_to(const ClassScheduledSection &other_section, const TravelMode mode) const noexcept{
        //         if(other_section.instruction_mode == InstructionMode::ONLINE) return true;
        //         if(other_section.room_building == Building::TBA) return true;
        //         const double time_between_classes_min = other_section.time.start_min - this->time.end_min;
        //         if(time_between_classes_min < 0) return true;
        //         return time_between_classes_min < time_between_buildings_min(mode, this->room_building, other_section.room_building);
        //     }
        // };

        // typedef void* PeopleSoftToken;
        // LL_API void login();
        LL_API void search_courses_base();
        LL_API void search_courses();
        // LL_API void swap_courses();
        // LL_API void course_history();
        // LL_API void add_course_to_shopping_cart();
        // LL_API std::unique_ptr<AcademicAdvisorReport> get_academic_advisor_report();
    };
}