
add_library(partitioning_partitioner_base SHARED partitioner_base.cc partitioner_base.h )

set(SOURCES
partitioning_partitioner_cc_proto
data_format_datapoint
data_format_dataset
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_scann_threadpool
oss_wrappers_tf_dependency
utils_types
utils_util_functions
utils_zip_sort

absl::base
absl::flags
)

target_link_libraries(partitioning_partitioner_base PUBLIC ${SOURCES})
 

add_library(partitioning_kmeans_tree_like_partitioner SHARED kmeans_tree_like_partitioner.h )

set(SOURCES
partitioning_partitioner_base
data_format_datapoint
distance_measures
trees_kmeans_tree
utils_types
)

target_link_libraries(partitioning_kmeans_tree_like_partitioner INTERFACE ${SOURCES})
 

add_library(partitioning_kmeans_tree_partitioner SHARED 
kmeans_tree_partitioner.cc 
kmeans_tree_partitioner_helper.cc 
kmeans_tree_partitioner.h )

set(SOURCES
partitioning_kmeans_tree_like_partitioner
partitioning_kmeans_tree_partitioner_cc_proto
partitioning_partitioner_base
partitioning_partitioner_cc_proto
base_search_parameters
base_single_machine_base
brute_force
distance_measures_many_to_many
hashes_asymmetric_hashing2_indexing
hashes_asymmetric_hashing2_querying
hashes_asymmetric_hashing2_searcher
hashes_asymmetric_hashing2_training
hashes_asymmetric_hashing2_training_options
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_tf_dependency
proto_distance_measure_cc_proto
proto_hash_cc_proto
proto_partitioning_cc_proto
trees_kmeans_tree
utils_common
utils_datapoint_utils
utils_fast_top_neighbors
utils_parallel_for
utils_types
utils_zip_sort

absl::base
absl::flags
absl::memory
absl::synchronization
)

target_link_libraries(partitioning_kmeans_tree_partitioner PUBLIC ${SOURCES})
 

add_library(partitioning_kmeans_tree_partitioner_utils SHARED kmeans_tree_partitioner_utils.h )

set(SOURCES
partitioning_kmeans_tree_partitioner
partitioning_partitioner_base
distance_measures
oss_wrappers_tf_dependency

absl::synchronization
)

target_link_libraries(partitioning_kmeans_tree_partitioner_utils INTERFACE ${SOURCES})
 

add_library(partitioning_projecting_decorator SHARED projecting_decorator.cc projecting_decorator.h )

set(SOURCES
partitioning_kmeans_tree_like_partitioner
partitioning_partitioner_base
oss_wrappers_tf_dependency
projection_projection_base
utils_datapoint_utils

absl::memory
)

target_link_libraries(partitioning_projecting_decorator PUBLIC ${SOURCES})
 

add_library(partitioning_partitioner_factory_base SHARED 
partitioner_factory_base.cc 
partitioner_factory_base.h )

set(SOURCES
partitioning_kmeans_tree_partitioner_utils
partitioning_partitioner_base
partitioning_projecting_decorator
data_format_dataset
distance_measures
oss_wrappers_tf_dependency
projection_projection_base
projection_projection_factory
proto_distance_measure_cc_proto
proto_partitioning_cc_proto
utils_types
utils_util_functions

absl::memory
absl::random_random
absl::time
)

target_link_libraries(partitioning_partitioner_factory_base PUBLIC ${SOURCES})
 

add_library(partitioning_partitioner_factory SHARED partitioner_factory.cc partitioner_factory.h )

set(SOURCES
partitioning_kmeans_tree_partitioner_cc_proto
partitioning_kmeans_tree_partitioner_utils
partitioning_partitioner_base
partitioning_partitioner_cc_proto
partitioning_partitioner_factory_base
partitioning_projecting_decorator
data_format_dataset
distance_measures
oss_wrappers_scann_random
oss_wrappers_tf_dependency
projection_projection_base
projection_projection_factory
proto_distance_measure_cc_proto
proto_partitioning_cc_proto
trees_kmeans_tree
utils_types
utils_util_functions
utils_weak_ptr_cache

absl::memory
absl::int128
absl::random_random
absl::strings
absl::time
)

target_link_libraries(partitioning_partitioner_factory PUBLIC ${SOURCES})


#-------------------------------------------------------------------------------

set(PROTO_SOURCES
partitioner.proto
linear_projection_tree.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(partitioning_partitioner_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(partitioning_partitioner_cc_proto PUBLIC 
partitioning_kmeans_tree_partitioner_cc_proto
)


set(PROTO_SOURCES
kmeans_tree_partitioner.proto
../trees/kmeans_tree/kmeans_tree.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(partitioning_kmeans_tree_partitioner_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(partitioning_kmeans_tree_partitioner_cc_proto PUBLIC
partitioning_partitioner_cc_proto
proto_partitioning_cc_proto
)
