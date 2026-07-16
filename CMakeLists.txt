cmake_minimum_required(VERSION 3.16)
project(privmatch-fetcher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(common)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB FETCHER_SOURCES "src/*.cpp")
add_executable(fetcher ${FETCHER_SOURCES} "src/game_modules.h" "src/game_modules.cpp")

target_include_directories(fetcher PRIVATE src)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(fetcher PRIVATE common CURL::libcurl spdlog::spdlog)