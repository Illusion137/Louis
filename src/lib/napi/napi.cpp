#include "napi.hpp"

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