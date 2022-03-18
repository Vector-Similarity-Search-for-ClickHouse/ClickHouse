cmake_minimum_required(VERSION 3.10)

project(scann)

# protobuf
INCLUDE(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# abslei 
add_subdirectory(abseil-cpp)

# tf
find_package(PkgConfig)
pkg_check_modules(TensorFlow REQUIRED tensorflow)

link_directories(${TensorFlow_LIBRARY_DIRS})
include_directories(${TensorFlow_INCLUDE_DIRS})

add_subdirectory(scann)

add_executable(test main.cpp)
target_link_libraries(test PUBLIC scann_ops_cc_scann) 