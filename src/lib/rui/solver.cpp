// #include <algorithm>
// #include <ranges>
// #include "solver.hpp"
// #include "distance.hpp"

// std::vector<peoplesoft::ClassScheduledSection*> possible_class_schedule_sections(){
//     return {};
//     // std::vector<peoplesoft::ClassScheduledSection*> all_sections = {};
//     // auto filtered_sections_view = all_sections | std::views::filter([](peoplesoft::ClassScheduledSection* section){
//     //     if(section->status == peoplesoft::ClassSectionStatus::CLOSED) return false;
//     //     if(section->status == peoplesoft::ClassSectionStatus::WAITLIST) return false;
//     //     // filter out prereqs; keep coreqs
//     //     // filter out already taken classes
//     //     // filter out classes not in timeframe
//     //     return true;
//     // });
//     // const auto filtered_sections = std::vector<peoplesoft::ClassScheduledSection*>(filtered_sections_view.begin(), filtered_sections_view.end());
//     // return filtered_sections;
// }

// rui::ClassSchedlueAdjacencyMatrix class_schedule_adjacency_matrix(const std::vector<peoplesoft::ClassScheduledSection*> &sections, TravelMode mode){
//     return {};
//     // const size_t sections_size = sections.size();

//     // rui::ClassSchedlueAdjacencyMatrix matrix;
//     // matrix.reserve(sections_size);

//     // for(int i = 0; i < sections_size; i++) {
//     //     matrix.push_back({});
//     //     matrix[i].reserve(sections_size);
//     //     for(int j = 0; j < sections_size; j++) {
//     //         matrix[i][j] = 0;
//     //         if(i != j && sections[i]->overlaps(*sections[j]) && sections[i]->can_make_it_to(*sections[j], mode))
//     //         {
//     //             matrix[i][j] = 1;
//     //         }
//     //     }
//     // }

//     // return matrix;
// }

// bool conflicting_sections(const peoplesoft::ClassScheduledSection* section, const peoplesoft::ClassScheduledSection* other_section){
//     if(section->overlaps(*other_section)) return true;
//     if(section->class_node->subject == other_section->class_node->subject && 
//         section->class_node->nbr == other_section->class_node->nbr) return true;
//     return false;
// }

// std::vector<peoplesoft::ClassScheduledSection*> rui::solve(const std::vector<peoplesoft::ClassScheduledSection*> &sections, const ClassSchedlueAdjacencyMatrix& matrix){
//     return {};
//     // constexpr uint32_t MAX_SCHEDULED_CLASSES = 16;
//     // constexpr float MAX_CREDITS = 19.0f;
//     // float credits = 0.0f;

//     // std::vector<peoplesoft::ClassScheduledSection*> found_class_schedule{};
//     // found_class_schedule.reserve(MAX_SCHEDULED_CLASSES);

//     // // sort first options : TODO maybe use a strange ass Max Heap, maybe also have an array of array of indices since
//     //     // we are only choosing like less than 16 classes
//     // std::vector<uint16_t> sections_indices;
//     // std::sort(sections_indices.begin(), sections_indices.end());

//     // // select the best indicy
//     // const uint16_t index = sections_indices[0];
//     // for(int i = 0; i < sections.size(); i++) {
//     //     // TODO maybe == 0 needs to be == 1
//     //     if(matrix[index][i] == 0) continue;
//     //     if(credits + sections[i]->class_node->credits > MAX_CREDITS) continue;
//     //     bool invalid_section = std::ranges::any_of(found_class_schedule, [&sections, i](auto section){
//     //         return conflicting_sections(section, sections[i]);
//     //     });
//     //     if(invalid_section) continue;
//     //     credits += sections[i]->class_node->credits;
//     // }

//     // if(credits >= MAX_CREDITS) return found_class_schedule;

//     // return found_class_schedule;
// }