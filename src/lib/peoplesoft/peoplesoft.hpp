#pragma once
#include <gen/campus.hpp>
#include <gen/courses.hpp>
#include <rui/distance.hpp>

// #include <iostream>
// #include <memory>
// #include <string>
// #include <cstdint>

namespace peoplesoft {
//     struct AcademicAdvisorReport_ProgramInformation {};
//     struct AcademicAdvisorReport_CurrentAcademicSummary {};
//     struct AcademicAdvisorReport_UniversityRequirements {};
//     struct AcademicAdvisorReport_InclusivePerspectives {};
//     struct AcademicAdvisorReport_GeneralStudies {};
//     struct AcademicAdvisorReport_GeneralStudiesRequirements {};
//     struct AcademicAdvisorReport_MajorRequirements {};
//     struct AcademicAdvisorReport_MinorRequirements {};
//     struct AcademicAdvisorReport_Electives {};
//     struct AcademicAdvisorReport {
//         std::string name; 
//         uint_fast16_t nau_id;
//     };
//     std::unique_ptr<AcademicAdvisorReport> get_academic_advisor_report();

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
//     std::unique_ptr<>
};