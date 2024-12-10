# Set project and minimum version
cmake_minimum_required(VERSION 3.2)
project(Alginate)

# Mark Alginate as a static library
add_library(Alginate STATIC
    src/Alginate.cpp
)

# Include Alginate header (including linked projects)
target_include_directories(Alginate
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Enable extensive compile errors
target_compile_options(Alginate
    PRIVATE
        -Wall
        -Wpedantic
        -Werror
)


# Temporary testing executable

add_executable(Test src/main.cpp)
target_link_libraries(Test Alginate)