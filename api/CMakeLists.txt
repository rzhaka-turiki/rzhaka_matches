cmake_minimum_required(VERSION 3.16)
project(match_api LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(libpqxx REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

set(PROTO_DIR ${CMAKE_SOURCE_DIR}/../proto)
file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_FILES})

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
)

target_link_libraries(match_api PRIVATE
    protobuf::libprotobuf
    gRPC::grpc++
    gRPC::grpc++_reflection
    libpqxx::pqxx
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)