cmake_minimum_required(VERSION 3.16)
project(cafe_tui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cafe_tui main.cpp)

target_include_directories(cafe_tui PRIVATE ${CMAKE_SOURCE_DIR})

# Optional: stricter warnings
target_compile_options(cafe_tui PRIVATE -Wall -Wextra)

# Static build option
option(STATIC_BUILD "Build statically linked executable" OFF)
if(STATIC_BUILD)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_options(cafe_tui PRIVATE -static-libgcc -static-libstdc++)
        # On Linux, compile with -static
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            target_link_options(cafe_tui PRIVATE -static)
        endif()
    endif()
endif()

# On macOS the std::filesystem may need extra linking
if(APPLE)
    target_link_libraries(cafe_tui PRIVATE)
endif()

# Install target
install(TARGETS cafe_tui DESTINATION bin)

