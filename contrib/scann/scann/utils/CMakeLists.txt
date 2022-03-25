add_subdirectory(fixed_point)
add_subdirectory(internal)
add_subdirectory(intrinsics)

add_library(utils_alignment SHARED alignment.cc alignment.h )

set(SOURCES
utils_common
)

target_link_libraries(utils_alignment PUBLIC ${SOURCES})
 

add_library(utils_types SHARED types.cc types.h )

set(SOURCES
utils_common
cc_proto

absl::core_headers
absl::flags
)

target_link_libraries(utils_types PUBLIC ${SOURCES})
 

add_library(utils_datapoint_utils INTERFACE)

set(SOURCES
utils_reduction
utils_types
data_format_datapoint
distance_measures_distance_measures
oss_wrappers_scann_bits
cc_proto
utils_intrinsics_flags
)

target_link_libraries(utils_datapoint_utils INTERFACE ${SOURCES})
 

add_library(utils_fast_top_neighbors SHARED fast_top_neighbors.cc fast_top_neighbors.h )

set(SOURCES
utils_bits
utils_common
utils_types
utils_util_functions
utils_zip_sort
oss_wrappers_scann_bits
utils_intrinsics_avx2
utils_intrinsics_flags
utils_intrinsics_sse4

absl::core_headers
absl::bits
absl::int128
absl::strings
)

target_link_libraries(utils_fast_top_neighbors PUBLIC ${SOURCES})
 

add_library(utils_gmm_utils SHARED gmm_utils.cc gmm_utils.h )

set(SOURCES
utils_common
utils_datapoint_utils
utils_fast_top_neighbors
utils_parallel_for
utils_top_n_amortized_constant
utils_types
utils_util_functions
utils_zip_sort
base_restrict_allowlist
data_format_datapoint
data_format_dataset
data_format_docid_collection
distance_measures_distance_measures
distance_measures_many_to_many
distance_measures_one_to_many
distance_measures_one_to_one_l2_distance
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_scann_random
oss_wrappers_scann_status
oss_wrappers_scann_threadpool
oss_wrappers_tf_dependency
cc_proto

absl::base
absl::endian
absl::flat_hash_set
absl::flags
absl::bits
absl::random_random
absl::random_distributions
absl::time
)

target_link_libraries(utils_gmm_utils PUBLIC ${SOURCES})
 

add_library(utils_infinite_one_array INTERFACE)

set(SOURCES
utils_types
)

target_link_libraries(utils_infinite_one_array INTERFACE ${SOURCES})
 

add_library(utils_bits INTERFACE)

set(SOURCES
utils_types

absl::bits
)

target_link_libraries(utils_bits INTERFACE ${SOURCES})
 

add_library(utils_index_sequence INTERFACE)


add_library(utils_reduction INTERFACE)

set(SOURCES
utils_types
data_format_datapoint
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_tf_dependency

absl::base

absl::flags
)

target_link_libraries(utils_reduction INTERFACE ${SOURCES})
 

add_library(utils_util_functions SHARED util_functions.cc util_functions.h )

set(SOURCES
utils_common
utils_parallel_for
utils_reduction
utils_types
utils_zip_sort
data_format_datapoint
data_format_internal_short_string_optimized_string
oss_wrappers_tf_dependency
cc_proto

absl::base
absl::flat_hash_map
absl::node_hash_set
)

target_link_libraries(utils_util_functions PUBLIC ${SOURCES})
 

add_library(utils_scann_config_utils SHARED scann_config_utils.cc scann_config_utils.h )

set(SOURCES
utils_common
utils_types
data_format_datapoint
distance_measures_distance_measures
oss_wrappers_scann_status
oss_wrappers_tf_dependency
cc_proto

absl::core_headers
absl::strings
absl::str_format
)

target_link_libraries(utils_scann_config_utils PUBLIC ${SOURCES})
 

add_library(utils_threads SHARED threads.cc threads.h )

set(SOURCES
utils_types
oss_wrappers_scann_threadpool

absl::strings
absl::time
)

target_link_libraries(utils_threads PUBLIC ${SOURCES})
 

add_library(utils_memory_logging SHARED memory_logging.cc memory_logging.h )

set(SOURCES
utils_types
data_format_datapoint
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_scann_malloc_extension
oss_wrappers_tf_dependency

absl::base
absl::node_hash_set
absl::flags
absl::strings
)

target_link_libraries(utils_memory_logging PUBLIC ${SOURCES})
 

add_library(utils_bit_iterator INTERFACE)

set(SOURCES
utils_types
oss_wrappers_scann_bits
)

target_link_libraries(utils_bit_iterator INTERFACE ${SOURCES})
 

add_library(utils_reordering_helper SHARED reordering_helper.cc reordering_helper.h )

set(SOURCES
utils_common
utils_datapoint_utils
utils_scalar_quantization_helpers
utils_types
utils_util_functions
base_single_machine_factory_options
data_format_datapoint
data_format_dataset
distance_measures_distance_measures
distance_measures_one_to_many
hashes_asymmetric_hashing2_querying
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_scann_status
oss_wrappers_tf_dependency
utils_fixed_point_pre_quantized_fixed_point
utils_internal_avx2_funcs
utils_internal_avx_funcs
utils_intrinsics_horizontal_sum

absl::base
absl::flags
absl::status
absl::str_format
)

target_link_libraries(utils_reordering_helper PUBLIC ${SOURCES})
 

add_library(utils_factory_helpers SHARED factory_helpers.cc factory_helpers.h )

set(SOURCES
utils_types
distance_measures_distance_measures
cc_proto
)

target_link_libraries(utils_factory_helpers PUBLIC ${SOURCES})
 

add_library(utils_hash_leaf_helpers SHARED hash_leaf_helpers.cc hash_leaf_helpers.h )

set(SOURCES
utils_factory_helpers
utils_types
base_single_machine_base
distance_measures_distance_measures
hashes_asymmetric_hashing2_indexing
hashes_asymmetric_hashing2_querying
hashes_asymmetric_hashing2_searcher
hashes_asymmetric_hashing2_training
hashes_asymmetric_hashing2_training_options
projection_projection_factory
cc_proto

absl::synchronization
)

target_link_libraries(utils_hash_leaf_helpers PUBLIC ${SOURCES})
 

add_library(utils_io_oss_wrapper SHARED io_oss_wrapper.cc io_oss_wrapper.h )

set(SOURCES
utils_common
)

target_link_libraries(utils_io_oss_wrapper PUBLIC ${SOURCES})
 

add_library(utils_io_npy SHARED io_npy.cc io_npy.h )

set(SOURCES
utils_io_oss_wrapper
data_format_dataset

absl::str_format
absl::span
)

target_link_libraries(utils_io_npy PUBLIC ${SOURCES})
 

add_library(utils_input_data_utils SHARED input_data_utils.cc input_data_utils.h )

set(SOURCES
data_format_dataset
cc_proto
utils_fixed_point_pre_quantized_fixed_point
)

target_link_libraries(utils_input_data_utils PUBLIC ${SOURCES})
 

add_library(utils_zip_sort INTERFACE)

set(SOURCES
utils_types
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_bits
oss_wrappers_tf_dependency
)

target_link_libraries(utils_zip_sort INTERFACE ${SOURCES})
 

add_library(utils_top_n_amortized_constant SHARED top_n_amortized_constant.cc top_n_amortized_constant.h )

set(SOURCES
utils_types
utils_util_functions
utils_zip_sort
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_tf_dependency

absl::base
absl::flags
)

target_link_libraries(utils_top_n_amortized_constant PUBLIC ${SOURCES})
 

add_library(utils_parallel_for INTERFACE)

set(SOURCES
utils_types
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_scann_threadpool
oss_wrappers_tf_dependency

absl::base
absl::core_headers
absl::flags
absl::function_ref
absl::status
absl::synchronization
)

target_link_libraries(utils_parallel_for INTERFACE ${SOURCES})
 

add_library(utils_dataset_sampling INTERFACE)

set(SOURCES
utils_types
data_format_dataset
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_scann_random
oss_wrappers_tf_dependency

absl::base
absl::flags
absl::random_random
absl::strings
absl::variant
)

target_link_libraries(utils_dataset_sampling INTERFACE ${SOURCES})
 

add_library(utils_iterators INTERFACE)

set(SOURCES
utils_types
)

target_link_libraries(utils_iterators INTERFACE ${SOURCES})
 

add_library(utils_scalar_quantization_helpers SHARED scalar_quantization_helpers.cc scalar_quantization_helpers.h )

set(SOURCES
utils_common
utils_datapoint_utils
utils_noise_shaping_utils
utils_top_n_amortized_constant
utils_types
utils_util_functions
utils_zip_sort
data_format_datapoint
data_format_dataset
)

target_link_libraries(utils_scalar_quantization_helpers PUBLIC ${SOURCES})
 

add_library(utils_common SHARED common.cc common.h)

set(SOURCES
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_scann_serialize
oss_wrappers_scann_status
oss_wrappers_tf_dependency

absl::base
absl::core_headers
absl::flat_hash_map
absl::flat_hash_set
absl::node_hash_map
absl::node_hash_set
absl::flags
absl::memory
absl::strings
absl::str_format
absl::span
)

target_link_libraries(utils_common PUBLIC ${SOURCES})
 

add_library(utils_noise_shaping_utils INTERFACE)

set(SOURCES
utils_types
)

target_link_libraries(utils_noise_shaping_utils INTERFACE ${SOURCES})
 

add_library(utils_weak_ptr_cache INTERFACE)

set(SOURCES
utils_types

absl::core_headers
absl::int128
absl::synchronization
)

target_link_libraries(utils_weak_ptr_cache INTERFACE ${SOURCES})