add_subdirectory(internal)

# Generated Files
# ==================================================================

set(PROTO_SOURCES
features.proto
../coscann/v2_restricts.proto
../proto/restricts.proto
${Protobuf_IMPORT_DIRS}/timestamp.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(data_format_features_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})


# Libraries
# ==================================================================

add_library(data_format_gfv_conversion SHARED gfv_conversion.h )

set(SOURCES
data_format_features_cc_proto
data_format_gfv_properties
utils_common
utils_types
)

target_link_libraries(data_format_gfv_conversion INTERFACE ${SOURCES})


add_library(data_format_datapoint SHARED datapoint.cc datapoint.h)

set(SOURCES
data_format_features_cc_proto
data_format_gfv_conversion
data_format_gfv_properties
oss_wrappers_tf_dependency
proto_hashed_cc_proto
utils_infinite_one_array
utils_types
utils_zip_sort

absl::strings
)

target_link_libraries(data_format_datapoint PUBLIC ${SOURCES})



add_library(data_format_dataset SHARED dataset.cc dataset.h)

set(SOURCES
data_format_datapoint
data_format_docid_collection
data_format_features_cc_proto
data_format_gfv_conversion
data_format_gfv_properties
data_format_sparse_low_level
distance_measures_distance_measure_base
distance_measures/one_to_one_l2_distance
oss_wrappers_scann_aligned_malloc
oss_wrappers_tf_dependency
proto_hashed_cc_proto
utils_common
utils_datapoint_utils
utils_iterators
utils_memory_logging
utils_types
utils_util_functions
utils_zip_sort

absl::flat_hash_set
absl::memory
absl::status
absl::strings
absl::time
)

target_link_libraries(data_format_dataset PUBLIC ${SOURCES})


add_library(data_format_docid_collection_interface SHARED docid_collection_interface.h )

set(SOURCES
oss_wrappers_scann_serialize
oss_wrappers_tf_dependency
utils_types

absl::strings
)

target_link_libraries(data_format_docid_collection_interface INTERFACE ${SOURCES})


add_library(data_format_docid_collection SHARED docid_collection.cc docid_collection.h)

set(SOURCES
data_format_docid_collection_interface
data_format_internal_short_string_optimized_string
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_scann_serialize
oss_wrappers_tf_dependency
utils_common
utils_memory_logging
utils_types
utils_util_functions

absl::base
absl::flat_hash_map
absl::node_hash_map
absl::flags
absl::status
absl::time
)

target_link_libraries(data_format_docid_collection PUBLIC ${SOURCES})


add_library(data_format_gfv_properties SHARED gfv_properties.cc gfv_properties.h)

set(SOURCES
data_format_features_cc_proto
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_tf_dependency
utils_types

absl::base
absl::flags
)

target_link_libraries(data_format_gfv_properties PUBLIC ${SOURCES})


add_library(data_format_sparse_low_level SHARED sparse_low_level.h)

set(SOURCES
utils_types
)

target_link_libraries(data_format_sparse_low_level INTERFACE ${SOURCES})

