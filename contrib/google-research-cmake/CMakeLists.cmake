
set(SCANN_ROOT_DIR "${ClickHouse_SOURCE_DIR}/contrib/google-research/scann")
set(SCANN_BIN_DIR "${ClickHouse_BINARY_DIR}/contrib/google-research-cmake/scann")

# add_subdirectory("${ClickHouse_SOURCE_DIR}/contrib/google-research/scann" "${ClickHouse_BINARY_DIR}/contrib/google-research/scann")

set (CMAKE_CXX_STANDARD 17)

add_compile_options(-mssse3 -msse4)
# set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_library(tensorflow SHARED IMPORTED)

set_target_properties(tensorflow PROPERTIES 
IMPORTED_LOCATION 
${ClickHouse_SOURCE_DIR}/contrib/google-research/scann/tensorflow/libtensorflow_cc.so
)

set(Protobuf_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/contrib/protobuf/src)
set(Protobuf_IMPORT_DIRS 
${CMAKE_SOURCE_DIR}/contrib/protobuf/src
${SCANN_ROOT_DIR}
)


set(Eigen3_DIR ${SCANN_ROOT_DIR}/eigen-3.4.0/cmake)
find_package(Eigen3 REQUIRED)

set(TensorFlow_INCLUDE_DIRS ${SCANN_ROOT_DIR}/tensorflow/tensorflow)
set(TensorFlow_LIBRARIES ${SCANN_ROOT_DIR}/tensorflow/libtensorflow_cc.so)

set(SCANN_INCLUDE_DIRS ${SCANN_ROOT_DIR}/scann_api CACHE PATH "")

set(INCLUDE_DIRS 
${SCANN_ROOT_DIR}
${SCANN_BIN_DIR}
${Protobuf_INCLUDE_DIRS}
${gtest_SOURCE_DIR}/include
${gtest_SOURCE_DIR}

${TensorFlow_INCLUDE_DIRS} 
${EIGEN3_INCLUDE_DIR} 
${Protobuf_INCLUDE_DIRS}
)

include_directories(${INCLUDE_DIRS})


set(PROTO_SOURCES
${SCANN_ROOT_DIR}/scann/data_format/features.proto
${SCANN_ROOT_DIR}/scann/partitioning/linear_projection_tree.proto
${SCANN_ROOT_DIR}/scann/partitioning/partitioner.proto
${SCANN_ROOT_DIR}/scann/partitioning/kmeans_tree_partitioner.proto

${SCANN_ROOT_DIR}/scann/proto/scann.proto
${SCANN_ROOT_DIR}/scann/proto/exact_reordering.proto
${SCANN_ROOT_DIR}/scann/proto/brute_force.proto
${SCANN_ROOT_DIR}/scann/proto/disjoint_restrict_token.proto
${SCANN_ROOT_DIR}/scann/proto/results.proto
${SCANN_ROOT_DIR}/scann/proto/crowding.proto
${SCANN_ROOT_DIR}/scann/proto/distance_measure.proto
${SCANN_ROOT_DIR}/scann/proto/projection.proto
${SCANN_ROOT_DIR}/scann/proto/metadata.proto
${SCANN_ROOT_DIR}/scann/proto/hash.proto
${SCANN_ROOT_DIR}/scann/proto/input_output.proto
${SCANN_ROOT_DIR}/scann/proto/restricts.proto
${SCANN_ROOT_DIR}/scann/proto/incremental_updates.proto
${SCANN_ROOT_DIR}/scann/proto/hashed.proto
${SCANN_ROOT_DIR}/scann/proto/partitioning.proto
${SCANN_ROOT_DIR}/scann/proto/centers.proto

${SCANN_ROOT_DIR}/scann/coscann/v2_restricts.proto
${SCANN_ROOT_DIR}/scann/trees/kmeans_tree/kmeans_tree.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(cc_proto ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(cc_proto PUBLIC
${ClickHouse_BINARY_DIR}/contrib/google-research-cmake
${SCANN_BIN_DIR}
${SCANN_BIN_DIR}/proto
${SCANN_BIN_DIR}/coscann
${SCANN_BIN_DIR}/trees/kmeans_tree
${SCANN_BIN_DIR}/partitioning
${SCANN_BIN_DIR}/data_format
${Protobuf_INCLUDE_DIRS}
)

add_library(all_source STATIC
${SCANN_ROOT_DIR}/scann/data_format/dataset.cc
${SCANN_ROOT_DIR}/scann/data_format/gfv_properties.cc
${SCANN_ROOT_DIR}/scann/data_format/datapoint.cc
${SCANN_ROOT_DIR}/scann/data_format/docid_collection.cc
${SCANN_ROOT_DIR}/scann/metadata/metadata_getter.cc
${SCANN_ROOT_DIR}/scann/brute_force/brute_force.cc
${SCANN_ROOT_DIR}/scann/brute_force/scalar_quantized_brute_force.cc
${SCANN_ROOT_DIR}/scann/partitioning/partitioner_factory.cc
${SCANN_ROOT_DIR}/scann/partitioning/partitioner_factory_base.cc
${SCANN_ROOT_DIR}/scann/partitioning/partitioner_base.cc
${SCANN_ROOT_DIR}/scann/partitioning/kmeans_tree_partitioner_helper.cc
${SCANN_ROOT_DIR}/scann/partitioning/kmeans_tree_partitioner.cc
${SCANN_ROOT_DIR}/scann/partitioning/projecting_decorator.cc
${SCANN_ROOT_DIR}/scann/tree_x_hybrid/tree_x_hybrid_smmd.cc
${SCANN_ROOT_DIR}/scann/tree_x_hybrid/tree_x_params.cc
${SCANN_ROOT_DIR}/scann/tree_x_hybrid/tree_ah_hybrid_residual.cc
${SCANN_ROOT_DIR}/scann/trees/kmeans_tree/kmeans_tree.cc
${SCANN_ROOT_DIR}/scann/trees/kmeans_tree/training_options.cc
${SCANN_ROOT_DIR}/scann/trees/kmeans_tree/kmeans_tree_node.cc
${SCANN_ROOT_DIR}/scann/hashes/asymmetric_hashing2/querying.cc
${SCANN_ROOT_DIR}/scann/hashes/asymmetric_hashing2/searcher.cc
${SCANN_ROOT_DIR}/scann/hashes/asymmetric_hashing2/indexing.cc
${SCANN_ROOT_DIR}/scann/hashes/asymmetric_hashing2/training_options_base.cc
${SCANN_ROOT_DIR}/scann/hashes/asymmetric_hashing2/training_options.cc
${SCANN_ROOT_DIR}/scann/hashes/asymmetric_hashing2/training_model.cc
${SCANN_ROOT_DIR}/scann/hashes/hashing_base.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/lut16_avx512_prefetch.tpl.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/lut16_avx512_noprefetch.tpl.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/write_distances_to_topn.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/lut16_avx512_swizzle.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/lut16_sse4.tpl.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/lut16_avx512_smart.tpl.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/stacked_quantizers.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/asymmetric_hashing_impl_omit_frame_pointer.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/lut16_avx2.tpl.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/asymmetric_hashing_impl.cc
${SCANN_ROOT_DIR}/scann/hashes/internal/lut16_interface.cc
${SCANN_ROOT_DIR}/scann/utils/scann_config_utils.cc
${SCANN_ROOT_DIR}/scann/utils/fast_top_neighbors.cc
${SCANN_ROOT_DIR}/scann/utils/memory_logging.cc
${SCANN_ROOT_DIR}/scann/utils/gmm_utils.cc
${SCANN_ROOT_DIR}/scann/utils/threads.cc
${SCANN_ROOT_DIR}/scann/utils/io_npy.cc
${SCANN_ROOT_DIR}/scann/utils/hash_leaf_helpers.cc
${SCANN_ROOT_DIR}/scann/utils/util_functions.cc
${SCANN_ROOT_DIR}/scann/utils/io_oss_wrapper.cc
${SCANN_ROOT_DIR}/scann/utils/scalar_quantization_helpers.cc
${SCANN_ROOT_DIR}/scann/utils/alignment.cc
${SCANN_ROOT_DIR}/scann/utils/intrinsics/flags.cc
${SCANN_ROOT_DIR}/scann/utils/input_data_utils.cc
${SCANN_ROOT_DIR}/scann/utils/types.cc
${SCANN_ROOT_DIR}/scann/utils/factory_helpers.cc
${SCANN_ROOT_DIR}/scann/utils/common.cc
${SCANN_ROOT_DIR}/scann/utils/top_n_amortized_constant.cc
${SCANN_ROOT_DIR}/scann/utils/reordering_helper.cc
${SCANN_ROOT_DIR}/scann/scann_ops/cc/scann.cc
${SCANN_ROOT_DIR}/scann/base/search_parameters.cc
${SCANN_ROOT_DIR}/scann/base/single_machine_base.cc
${SCANN_ROOT_DIR}/scann/base/restrict_allowlist.cc
${SCANN_ROOT_DIR}/scann/base/single_machine_factory_options.cc
${SCANN_ROOT_DIR}/scann/base/reordering_helper_factory.cc
${SCANN_ROOT_DIR}/scann/base/single_machine_factory_scann.cc
${SCANN_ROOT_DIR}/scann/oss_wrappers/scann_bits.cc
${SCANN_ROOT_DIR}/scann/oss_wrappers/scann_status_builder.cc
${SCANN_ROOT_DIR}/scann/oss_wrappers/scann_serialize.cc
${SCANN_ROOT_DIR}/scann/oss_wrappers/scann_aligned_malloc.cc
${SCANN_ROOT_DIR}/scann/oss_wrappers/scann_status.cc
${SCANN_ROOT_DIR}/scann/distance_measures/many_to_many/many_to_many_flags.cc
${SCANN_ROOT_DIR}/scann/distance_measures/many_to_many/fp8_transposed.cc
${SCANN_ROOT_DIR}/scann/distance_measures/many_to_many/many_to_many_double_top1.cc
${SCANN_ROOT_DIR}/scann/distance_measures/many_to_many/many_to_many_float_top1.cc
${SCANN_ROOT_DIR}/scann/distance_measures/many_to_many/many_to_many_float_results.cc
${SCANN_ROOT_DIR}/scann/distance_measures/many_to_many/many_to_many_double_results.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/cosine_distance.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/dot_product_sse4.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/l1_distance_sse4.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/l2_distance.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/l2_distance_avx1.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/dot_product_avx2.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/limited_inner_product.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/binary_distance_measure_base.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/l2_distance_sse4.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/dot_product_avx1.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/nonzero_intersect_distance.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/jaccard_distance.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/l1_distance.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/hamming_distance.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_one/dot_product.cc
${SCANN_ROOT_DIR}/scann/distance_measures/distance_measure_base.cc
${SCANN_ROOT_DIR}/scann/distance_measures/distance_measure_factory.cc
${SCANN_ROOT_DIR}/scann/distance_measures/one_to_many/one_to_many.cc
${SCANN_ROOT_DIR}/scann/projection/projection_factory.cc
${SCANN_ROOT_DIR}/scann/projection/identity_projection.cc
${SCANN_ROOT_DIR}/scann/projection/chunking_projection.cc
${SCANN_ROOT_DIR}/scann/projection/projection_base.cc
${SCANN_ROOT_DIR}/scann/projection/random_orthogonal_projection.cc
)

target_link_libraries(all_source PUBLIC 
cc_proto 
${TensorFlow_LIBRARIES}
tensorflow
# tensorflow_framework

absl::core_headers
absl::flags
absl::base
absl::memory
# absl::string
absl::flat_hash_set
absl::flat_hash_map
absl::status
absl::time
absl::random_random
absl::random_distributions
)

add_library(scann_interface STATIC
${SCANN_ROOT_DIR}/scann_api/scann/scann_searcher.cpp
${SCANN_ROOT_DIR}/scann_api/scann/scann_builder.cpp
# ${SCANN_ROOT_DIR}/scann/scann_ops/cpp_interface/scann_builder.cpp
# ${SCANN_ROOT_DIR}/scann/scann_ops/cpp_interface/scann_ops_pybind.cpp
)

target_link_libraries(scann_interface PUBLIC
all_source 
tensorflow
# tensorflow_framework

cc_proto 
${TensorFlow_LIBRARIES}

absl::core_headers
absl::flags
absl::base
absl::memory
# absl::string
absl::flat_hash_set
absl::flat_hash_map
absl::status
absl::time
absl::random_random
absl::random_distributions
)




add_library(_scann INTERFACE)
target_link_libraries(_scann INTERFACE all_source scann_interface tensorflow)
target_include_directories(_scann SYSTEM INTERFACE ${SCANN_INCLUDE_DIRS})

add_library(ch_contrib::scann ALIAS _scann)
