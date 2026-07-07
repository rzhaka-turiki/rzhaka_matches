cmake_minimum_required(VERSION 3.16)
project(privmatch-fetcher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

add_executable(fetcher
    src/main.cpp
    src/api.cpp
    src/processor.cpp
)

target_include_directories(fetcher PRIVATE ${PQXX_INCLUDE_DIRS})
target_link_libraries(fetcher PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
    ${PQXX_LIBRARIES}
    spdlog::spdlog
)