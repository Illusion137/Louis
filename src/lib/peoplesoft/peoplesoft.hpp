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
    struct ClassScheduledSection {
        ClassNode *class_node;
        uint8_t section_id;
        uint16_t class_id; // this is unique to every class time
        ClassSectionDays days;
        ClassSectionTime time;
        Building room_building;
        uint16_t room_no;
        char room_no_suffix[4];
        // Instructor instructor;
        Date meeting_date_start;
        Date meeting_date_end;
        Campus campus;
        InstructionMode instruction_mode;
        uint16_t available_seats;
        bool reserved_seating;
        ClassSectionStatus status;
        char sun_id[32];
        bool overlaps(const ClassScheduledSection &other_section) const noexcept{
            if(this->instruction_mode == InstructionMode::ONLINE) return false;
            if(other_section.instruction_mode == InstructionMode::ONLINE) return false;
            // TODO check meeting_date_start and meeting date end
                // two 8 weeks on opposite sides can't overlap
            const uint32_t shared_days = this->days & other_section.days;
            if(shared_days == 0) return false;
            return this->time.overlaps(other_section.time);
        }
        // NOTE: this doesn't check for overlap because it's assume to already be checked
        bool can_make_it_to(const ClassScheduledSection &other_section, const TravelMode mode) const noexcept{
            if(other_section.instruction_mode == InstructionMode::ONLINE) return true;
            if(other_section.room_building == Building::TBA) return true;
            const double time_between_classes_min = other_section.time.start_min - this->time.end_min;
            if(time_between_classes_min < 0) return true;
            return time_between_classes_min < time_between_buildings_min(mode, this->room_building, other_section.room_building);
        }
    };
//     std::unique_ptr<>
};