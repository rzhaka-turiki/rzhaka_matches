cmake_minimum_required(VERSION 3.16)
project(match_api LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../common common_build)

find_package(Protobuf REQUIRED)

if(WIN32)
    find_package(gRPC CONFIG REQUIRED)
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)
    find_program(PROTOC protoc REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GRPC REQUIRED grpc++ grpc)
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)
    find_program(PROTOC protoc REQUIRED)
endif()

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

if(NOT DEFINED PROTO_DIR)
    set(PROTO_DIR ${CMAKE_SOURCE_DIR}/../proto)
endif()

set(PROTO_FILE ${PROTO_DIR}/match_service.proto)
set(PROTO_SRC ${CMAKE_CURRENT_BINARY_DIR}/match_service.pb.cc)
set(GRPC_SRC ${CMAKE_CURRENT_BINARY_DIR}/match_service.grpc.pb.cc)

add_custom_command(
    OUTPUT ${PROTO_SRC} ${GRPC_SRC}
    COMMAND ${PROTOC}
        --proto_path=${PROTO_DIR}
        --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
        --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating gRPC code for match_service.proto"
)

add_executable(match_api
    main_api.cpp
    grpc_service.cpp
    ${PROTO_SRC}
    ${GRPC_SRC}
)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(WIN32)
    target_include_directories(match_api PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROTOBUF_INCLUDE_DIRS}
    )
    target_link_libraries(match_api PRIVATE
        ${PROTOBUF_LIBRARIES}
        gRPC::grpc++
        spdlog::spdlog
        common
    )
else()
    target_include_directories(match_api PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROTOBUF_INCLUDE_DIRS}
        ${GRPC_INCLUDE_DIRS}
    )
    target_link_libraries(match_api PRIVATE
        ${PROTOBUF_LIBRARIES}
        ${GRPC_LINK_LIBRARIES}
        spdlog::spdlog
        common
    )
endif()