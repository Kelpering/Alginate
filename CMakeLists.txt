# Set project and minimum version
cmake_minimum_required(VERSION 3.2)
project(Alginate)

# Exports compile_commands.json (which clangd uses)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add all source files to "sources"
file(GLOB_RECURSE sources "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Mark Alginate as a static library
add_library(Alginate STATIC
    ${sources}
)

# Include Alginate header (public for linked projects too)
target_include_directories(Alginate
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Enable extensive compile errors and optimizations
target_compile_options(Alginate
    PRIVATE
        -Wall
        -Wpedantic
        -Werror
        -ggdb
        -O2
)


# Temporary testing executable (with extensive compile warnings)
add_executable(Test ./main.cpp)
target_compile_options(Test
    PRIVATE
        -Wall
        -Wpedantic
        -ggdb
        -O2
)
target_link_libraries(Test Alginate)