#pragma once
#include <frozen/unordered_map.h>
#include <frozen/string.h>

enum class CatalogYears {
	Y1516,
	Y1617,
	Y1718,
	Y1819,
	Y1920,
	Y2021,
	Y2122,
	Y2223,
	Y2324,
	Y2425,
	Y2526,
	Y2627,
};

constexpr frozen::unordered_map<frozen::string, CatalogYears, 12> catalogyears_fstrmap = {
	{"2015-2016", CatalogYears::Y1516},
	{"2016-2017", CatalogYears::Y1617},
	{"2017-2018", CatalogYears::Y1718},
	{"2018-2019", CatalogYears::Y1819},
	{"2019-2020", CatalogYears::Y1920},
	{"2020-2021", CatalogYears::Y2021},
	{"2021-2022", CatalogYears::Y2122},
	{"2022-2023", CatalogYears::Y2223},
	{"2023-2024", CatalogYears::Y2324},
	{"2024-2025", CatalogYears::Y2425},
	{"2025-2026", CatalogYears::Y2526},
	{"2026-2027", CatalogYears::Y2627},
};

constexpr frozen::unordered_map<CatalogYears, frozen::string, 12> catalogyears_tstrmap = {
	{CatalogYears::Y1516, "2015-2016"},
	{CatalogYears::Y1617, "2016-2017"},
	{CatalogYears::Y1718, "2017-2018"},
	{CatalogYears::Y1819, "2018-2019"},
	{CatalogYears::Y1920, "2019-2020"},
	{CatalogYears::Y2021, "2020-2021"},
	{CatalogYears::Y2122, "2021-2022"},
	{CatalogYears::Y2223, "2022-2023"},
	{CatalogYears::Y2324, "2023-2024"},
	{CatalogYears::Y2425, "2024-2025"},
	{CatalogYears::Y2526, "2025-2026"},
	{CatalogYears::Y2627, "2026-2027"},
};