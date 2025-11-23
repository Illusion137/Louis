#include <glaze/glaze.hpp>
#include <cpr/cpr.h>
#include <sll.hpp>
#include <rozfetch.hpp>
#include <vector>

#define DAYS_IN_WEEK 7

#define NAPI_EXTENDED_BASE_URL "https://api.extended.nau.edu/api/"

namespace napi {
    enum CourseAttribute {
        COURSE_ATTRIBUTE_NONE = 0,
        COURSE_ATTRIBUTE_DIV = 1,
        COURSE_ATTRIBUTE_GENS = 1 << 1,
        COURSE_ATTRIBUTE_DIVP = 1 << 2,
        COURSE_ATTRIBUTE_LIBS = 1 << 3,
    };

    namespace connect {
        struct Campuses {
            struct Campus {
                std::string Value;
                std::string Text;
            };
            std::vector<Campus> campus;
        };
        std::vector<Campuses::Campus> get_campuses();
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
        std::vector<CatalogYears::CatalogYears_catalogYears> get_catalog_years();
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
        CourseCatalog get_course_catalog();
    };
}