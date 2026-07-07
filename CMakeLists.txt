cmake_minimum_required(VERSION 3.16)
project(privmatch-fetcher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(libpqxx REQUIRED)
find_package(spdlog REQUIRED)

add_executable(fetcher
    src/main.cpp
    src/config.cpp
    src/database.cpp
    src/api_client.cpp
    src/match_processor.cpp
    src/scheduler.cpp
)

target_include_directories(fetcher PRIVATE src)

target_link_libraries(fetcher PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
    libpqxx::pqxx
    spdlog::spdlog
)