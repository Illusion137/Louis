#include "napi.hpp"
#include <libxml2/libxml/HTMLparser.h>
#include <libxml2/libxml/xpath.h>
#include <vector>

std::vector<napi::connect::Campuses::Campus> napi::connect::get_campuses(){
    const auto campuses = 
        ll::rozfetch_get_json<napi::connect::Campuses>(NAPI_EXTENDED_BASE_URL"connect/Campuses");
    if(campuses.has_value()) return campuses.value().campus;
    return {};
}
std::vector<napi::transfer_plan::CatalogYears::CatalogYears_catalogYears> napi::transfer_plan::get_catalog_years(){
    const auto catalog_years = 
        ll::rozfetch_get_json<napi::transfer_plan::CatalogYears>(NAPI_EXTENDED_BASE_URL"TransferPlan/GetCatalogYears");
    if(catalog_years.has_value()) return catalog_years.value().catalogYears;
    return {};
}

napi::course_catalog::CourseCatalog napi::course_catalog::get_course_catalog(){
    const auto course_catalog = 
        ll::rozfetch_get_json<napi::course_catalog::CourseCatalog>(NAPI_EXTENDED_BASE_URL"CourseCatalog");
    if(course_catalog.has_value()) return course_catalog.value();
    return {};
}

std::vector<napi::edu::DropdownOption> extract_catalog_dropdown_data(const xmlXPathContextPtr context, const xmlChar *xpath){
    std::vector<napi::edu::DropdownOption> dropdown;

    xmlXPathObjectPtr dropdown_elements = xmlXPathEvalExpression(xpath, context);
    const auto &nodeset = dropdown_elements->nodesetval;
    for (int i = 0; i < nodeset->nodeNr; ++i){
        const std::string text = std::string{(char*)xmlNodeGetContent(nodeset->nodeTab[i])};
        const std::string value = std::string{(char*)xmlGetProp(nodeset->nodeTab[i], (xmlChar *)"value")};

        dropdown.emplace_back(text, value);
    }
    return dropdown;
}
napi::edu::CourseDropdowns napi::edu::get_catalog_dropdowns(){
    const auto catalog_html = ll::rozfetch_get_text(NAPI_EDU_BASE_URL"Courses/");
    if(!catalog_html.has_value()) return {{},{},{}};
    htmlDocPtr doc = htmlReadMemory(catalog_html.value().c_str(), catalog_html.value().length(), nullptr, nullptr, HTML_PARSE_NOERROR);
    xmlXPathContextPtr context = xmlXPathNewContext(doc);

    const auto catalog_terms_dropdown = extract_catalog_dropdown_data(context, (xmlChar*)"//*[@id=\"term1\"]/option");
    const auto course_attributes_dropdown = extract_catalog_dropdown_data(context, (xmlChar*)"//*[@id=\"crseAttr\"]/option");
    const auto course_attributes_values_dropdown = extract_catalog_dropdown_data(context, (xmlChar*)"//*[@id=\"crseAttrValue\"]/option");

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);

    return {
        .catalog_terms_dropdown = std::move(catalog_terms_dropdown),
        .course_attributes_dropdown = std::move(course_attributes_dropdown),
        .course_attributes_values_dropdown = std::move(course_attributes_values_dropdown)
    };
}

std::vector<napi::edu::CourseResult> napi::edu::get_course_attribute_courses(const CatalogTerms term){
    // TODO convert term to string; Dont hardcode it dickhead
    const auto courses_html = ll::rozfetch_get_text(NAPI_EDU_BASE_URL"Courses/results?crseAttr=GENS&crseAttrValue=01&term=1264");
    if(!courses_html.has_value()) return {};
    std::vector<CourseResult> course_results;
    
    htmlDocPtr doc =
        htmlReadMemory(courses_html.value().c_str(), courses_html.value().length(), nullptr, nullptr, HTML_PARSE_NOERROR);
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    
    xmlXPathObjectPtr course_elements = xmlXPathEvalExpression((xmlChar *) "//*[@id=\"results-list\"]/dt", context);
    course_results.reserve(course_elements->nodesetval->nodeNr);

    for (int i = 0; i < course_elements->nodesetval->nodeNr; ++i){
        // TODO maybe parse more???
        const std::string subject = std::string((char*)(xmlGetProp(course_elements->nodesetval->nodeTab[i], (xmlChar *)"subject")));
        const std::string catnum = std::string((char*)(xmlGetProp(course_elements->nodesetval->nodeTab[i], (xmlChar *)"catnum")));
        const std::string crseattr = std::string((char*)(xmlGetProp(course_elements->nodesetval->nodeTab[i], (xmlChar *)"crseattr")));

        CourseResult result = {.crseattr = crseattr, .subject=subject, .catnum=catnum};
        course_results.push_back(result);
    }

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    
    return course_results;
}

// void napi::peoplesoft::login(){

// }
void napi::peoplesoft::search_courses_base(){
    const auto course_base_html = ll::rozfetch_get_text(NAPI_PEOPLESOFT_BASE_URL"psc/ps92prcs/EMPLOYEE/SA/c/COMMUNITY_ACCESS.CLASS_SEARCH.GBL?Page=SSR_CLSRCH_ENTRY&inst=NAU00&open=N&search=true&");
}

void napi::peoplesoft::search_courses(){
    const SearchCoursesParams params = create_search_course_params(CatalogTerms::Y1516, CourseSubject::CS);
    const auto courses_html = ll::rozfetch_post_response<SearchCoursesParams>(
        NAPI_PEOPLESOFT_BASE_URL"psc/ps92prcs/EMPLOYEE/SA/c/COMMUNITY_ACCESS.CLASS_SEARCH.GBL", 
        params
        // {}
        );
    if(courses_html){
        LL_INFO("{}", courses_html.value().text);
    }
}
// void napi::peoplesoft::swap_courses(){

// }
// void napi::peoplesoft::course_history(){

// }
// void napi::peoplesoft::add_course_to_shopping_cart(){
    
// }
// std::unique_ptr<napi::peoplesoft::AcademicAdvisorReport> napi::peoplesoft::get_academic_advisor_report(){
//     // return {};
// }