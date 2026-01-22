cmake_minimum_required(VERSION 3.20)
project(louis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE SRC_FILES "${SOURCE_DIR}/lib/*.cpp")

include(FetchContent)

# Glaze (JSON)
FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glaze)

# Catch2 (testing)
# git clone https://github.com/libcpr/cpr.git\ncd cpr && mkdir build && cd build\ncmake .. -DCPR_USE_SYSTEM_CURL=ON -DBUILD_SHARED_LIBS=OFF\ncmake --build . --parallel\nsudo cmake --install .
find_package(Catch2 REQUIRED)

# libxml2 (HTML)
find_package(LibXml2 REQUIRED)

# OpenSSL (SSL)
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)

# Frozen (HashMap)
FetchContent_Declare(
    frozen
    GIT_REPOSITORY https://github.com/serge-sans-paille/frozen.git
    GIT_TAG 1.2.0
)
FetchContent_MakeAvailable(frozen)

# CPR (HTTP networking)
set(CPR_USE_SYSTEM_CURL ON)
set(CPR_BUILD_TESTS OFF CACHE BOOL "Don't build CPR tests")
set(CPR_FORCE_USE_SYSTEM_CURL OFF CACHE BOOL "Use built-in curl if needed")
find_package(cpr REQUIRED)

# FetchContent_Declare(
#     cpr
#     GIT_REPOSITORY https://github.com/libcpr/cpr.git
#     GIT_TAG        1.13.0   # Latest stable as of 2025; change if needed
# )
# FetchContent_MakeAvailable(cpr)

# ============================
# Executable
# ============================
add_executable(Louis
    src/main.cpp
    ${SRC_FILES}
)
add_executable(PrebuildLouis
    src/prebuild.cpp
    ${SRC_FILES}
)
# add_executable(TestLouis
#     src/test.cpp
#     ${SRC_FILES}
# )

target_compile_options(Louis PRIVATE -Wall -O3 -Wno-reorder-init-list)
target_compile_options(PrebuildLouis PRIVATE -Wall -Wno-reorder-init-list)
# target_compile_options(TestLouis PRIVATE -Wall --coverage)

# Link dependencies
target_link_libraries(Louis
    PRIVATE
    cpr::cpr
    glaze::glaze
    LibXml2::LibXml2
    OpenSSL::Crypto OpenSSL::SSL
    frozen
)
target_link_libraries(PrebuildLouis
    PRIVATE
    cpr::cpr
    glaze::glaze
    LibXml2::LibXml2
    OpenSSL::Crypto OpenSSL::SSL
    frozen
)
# target_link_libraries(TestLouis
#     PRIVATE
#     cpr::cpr
#     glaze::glaze
#     LibXml2::LibXml2
#     OpenSSL::Crypto OpenSSL::SSL
#     frozen
#     Catch2::Catch2WithMain
# )

target_include_directories(Louis
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/lib
    ${glaze_SOURCE_DIR}/include
    /Users/illusion/dev/cpr/include/
)
target_include_directories(PrebuildLouis
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/lib
    ${glaze_SOURCE_DIR}/include
    /Users/illusion/dev/cpr/include/
)

target_compile_features(Louis PRIVATE cxx_std_23)
target_compile_features(PrebuildLouis PRIVATE cxx_std_23)

# target_include_directories(TestLouis
#     PRIVATE
#     ${PROJECT_SOURCE_DIR}/src/lib
#     ${glaze_SOURCE_DIR}/include
#    /Users/illusion/dev/cpr/include/
# )

# include(CTest)
# include(Catch)
# catch_discover_tests(TestLouis)