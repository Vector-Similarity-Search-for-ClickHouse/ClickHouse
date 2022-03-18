add_library(tree_x_hybrid_internal_utils SHARED utils.h )

set(SOURCES
base_restrict_allowlist
base_search_parameters
base_single_machine_base
hashes_asymmetric_hashing2_searcher
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_tf_dependency
utils_types

absl::base
absl::flags
)

target_link_libraries(tree_x_hybrid_internal_utils INTERFACE ${SOURCES})
 

add_library(tree_x_hybrid_internal_batching SHARED batching.h )

set(SOURCES
base_search_parameters
data_format_dataset
utils_fast_top_neighbors
utils_types
)

target_link_libraries(tree_x_hybrid_internal_batching INTERFACE ${SOURCES})