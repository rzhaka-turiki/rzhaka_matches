cmake_minimum_required(VERSION 3.16)
project(common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)

if(WIN32)
    find_package(libpqxx CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PQXX REQUIRED libpqxx)
endif()

add_library(common STATIC
    config.cpp
    database_base.cpp
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_link_libraries(common PUBLIC libpqxx::pqxx nlohmann_json::nlohmann_json)
else()
    target_link_libraries(common PUBLIC ${PQXX_LIBRARIES} nlohmann_json::nlohmann_json)
    target_include_directories(common PUBLIC ${PQXX_INCLUDE_DIRS})
endif()