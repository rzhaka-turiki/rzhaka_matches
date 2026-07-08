cmake_minimum_required(VERSION 3.16)
project(match_api LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)

# gRPC через pkg-config
pkg_check_modules(GRPC REQUIRED grpc++ grpc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)

# libpqxx через pkg-config (избегаем проблем с CMake-конфигом)
pkg_check_modules(PQXX REQUIRED libpqxx)

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

# Путь к proto-файлам
if(NOT DEFINED PROTO_DIR)
    set(PROTO_DIR ${CMAKE_SOURCE_DIR}/../proto)
endif()

file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")
find_program(PROTOC protoc REQUIRED)

foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    set(PROTO_SRC ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.cc)
    set(GRPC_SRC ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.cc)
    list(APPEND PROTO_SRCS ${PROTO_SRC})
    list(APPEND GRPC_SRCS ${GRPC_SRC})

    add_custom_command(
        OUTPUT ${PROTO_SRC} ${GRPC_SRC}
        COMMAND ${PROTOC}
            --proto_path=${PROTO_DIR}
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
            ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        COMMENT "Generating C++ code for ${PROTO_NAME}"
    )
endforeach()

add_executable(match_api
    main_api.cpp
    grpc_service.cpp
    database.cpp
    config.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_include_directories(match_api PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIRS}
)

target_link_libraries(match_api PRIVATE
    ${PROTOBUF_LIBRARIES}
    ${GRPC_LINK_LIBRARIES}
    ${PQXX_LIBRARIES}
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)