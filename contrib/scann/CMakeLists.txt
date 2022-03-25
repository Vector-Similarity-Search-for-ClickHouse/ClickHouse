cmake_minimum_required(VERSION 3.10)

project(scann)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-mssse3 -msse4)

#gtest 
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# protobuf
INCLUDE(FindProtobuf)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules")
find_package(Protobuf REQUIRED)

# abslei 
set(ABSL_PROPAGATE_CXX_STD ON)
add_subdirectory(abseil-cpp)

# eigen3
set(Eigen3_DIR /home/nikita/Downloads/eigen-3.4.0/cmake)
find_package(Eigen3 REQUIRED)

# TensorFlow
find_package(TensorFlow REQUIRED)


set(Protobuf_IMPORT_DIRS ${PROJECT_SOURCE_DIR})


set(PROJECT_INCLUDE_DIRS 
${PROJECT_SOURCE_DIR} 
${PROJECT_BINARY_DIR} 
${EIGEN3_INCLUDE_DIR} 
${TensorFlow_INCLUDE_DIRS}  
${Protobuf_INCLUDE_DIRS}
${gtest_SOURCE_DIR}/include
${gtest_SOURCE_DIR}
)

include_directories(${PROJECT_INCLUDE_DIRS})


set(PROTO_SOURCES
scann/data_format/features.proto
scann/partitioning/linear_projection_tree.proto
scann/partitioning/partitioner.proto
scann/partitioning/kmeans_tree_partitioner.proto
scann/proto/scann.proto
scann/proto/exact_reordering.proto
scann/proto/brute_force.proto
scann/proto/disjoint_restrict_token.proto
scann/proto/results.proto
scann/proto/crowding.proto
scann/proto/distance_measure.proto
scann/proto/projection.proto
scann/proto/metadata.proto
scann/proto/hash.proto
scann/proto/input_output.proto
scann/proto/restricts.proto
scann/proto/incremental_updates.proto
scann/proto/hashed.proto
scann/proto/partitioning.proto
scann/proto/centers.proto
scann/coscann/v2_restricts.proto
scann/trees/kmeans_tree/kmeans_tree.proto)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(cc_proto ${PROTO_SRCS} ${PROTO_HDRS})

add_subdirectory(scann)

add_executable(mian main.cpp)
target_link_libraries(mian PUBLIC scann_ops_cc_scann) 