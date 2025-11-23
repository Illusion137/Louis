#include <peoplesoft/peoplesoft.hpp>

namespace rui {
    typedef std::vector<std::vector<uint8_t>> ClassSchedlueAdjacencyMatrix;

    struct ScheduleConstraint {

    };

    std::vector<peoplesoft::ClassScheduledSection*> possible_class_schedule_sections();
    ClassSchedlueAdjacencyMatrix class_schedule_adjacency_matrix(const std::vector<peoplesoft::ClassScheduledSection*> &sections, TravelMode mode);

    std::vector<peoplesoft::ClassScheduledSection*> solve(const std::vector<peoplesoft::ClassScheduledSection*> &sections, const ClassSchedlueAdjacencyMatrix& matrix);
    
    // void bar(){
        // generate the vector of all ClassScheduledSections while filtering impossible classes
            // classes already taken or currently taking
            // this includes things that require prereqs but NOT coreqs
            // things that make no sense for the given array of constraints
        // sort the vector by classes being "desireable"
        // we want to generate an adjacency matrix of all possible ClassScheduledSections
        // take the best possible class-section and try to select it, we'll come back if this path fails
        // if the class has a coreq immediately try to pick from there
        // eliminate class-sections of the same class and any class-sections with overlapping time
        // anything that'll take take more than 19 credits of weight for the edges gotta go so we can begin to fail early and
        // give the current schedule scores foreach schedule-like the user provided
        // cycle: pick another class and if it leads to permanant conflict find the common demoninator tell the user
            // and hop bottom backtracking
    // }
}