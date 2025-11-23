cmake_minimum_required(VERSION 3.20)
project(louis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE SRC_FILES "${SOURCE_DIR}/lib/*.cpp")

include(FetchContent)

# Glaze (JSON)
FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(glaze)

# Lexbor (HTML)
FetchContent_Declare(
    lexbor
    GIT_REPOSITORY https://github.com/lexbor/lexbor.git
    GIT_TAG v2.6.0
)
FetchContent_MakeAvailable(lexbor)

# Frozen (HashMap)
FetchContent_Declare(
    frozen
    GIT_REPOSITORY https://github.com/serge-sans-paille/frozen.git
    GIT_TAG 1.2.0
)
FetchContent_MakeAvailable(frozen)

# CPR (HTTP networking)
set(CPR_BUILD_TESTS OFF CACHE BOOL "Don't build CPR tests")
set(CPR_FORCE_USE_SYSTEM_CURL OFF CACHE BOOL "Use built-in curl if needed")

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.13.0   # Latest stable as of 2025; change if needed
)

FetchContent_MakeAvailable(cpr)

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

target_compile_options(Louis PRIVATE -Wall)
target_compile_options(PrebuildLouis PRIVATE -Wall)

# Link dependencies
target_link_libraries(Louis
    PRIVATE
    cpr::cpr
    glaze::glaze
    lexbor
    frozen
)
target_link_libraries(PrebuildLouis
    PRIVATE
    cpr::cpr
    glaze::glaze
    lexbor
    frozen
)

target_include_directories(Louis
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/lib
    ${glaze_SOURCE_DIR}/include
    ${LEXBOR_INCLUDE_DIRS}
)
target_include_directories(PrebuildLouis
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/lib
    ${glaze_SOURCE_DIR}/include
    ${LEXBOR_INCLUDE_DIRS}
)
