cmake_minimum_required(VERSION 3.16)
project(privmatch-fetcher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
if(WIN32)
    find_package(libpqxx CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PQXX REQUIRED libpqxx)
endif()

file(GLOB FETCHER_SOURCES "src/*.cpp")

add_executable(fetcher ${FETCHER_SOURCES})

if(WIN32)
    target_include_directories(fetcher PRIVATE src)
    target_link_libraries(fetcher PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
        libpqxx::pqxx
        spdlog::spdlog
    )
else()
    target_include_directories(fetcher PRIVATE src ${PQXX_INCLUDE_DIRS})
    target_link_libraries(fetcher PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
        ${PQXX_LIBRARIES}
        spdlog::spdlog
    )
endif()