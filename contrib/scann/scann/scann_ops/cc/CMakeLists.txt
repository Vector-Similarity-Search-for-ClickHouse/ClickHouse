
add_library(scann_ops_cc_scann SHARED scann.cc scann.h )

set(SOURCES
base_search_parameters
base_single_machine_base
base_single_machine_factory_options
base_single_machine_factory_scann
data_format_dataset
oss_wrappers_tf_dependency
oss_wrappers_scann_status
cc_proto
tree_x_hybrid_tree_x_params
utils_io_npy
utils_io_oss_wrapper
utils_scann_config_utils
utils_threads

absl::base
absl::flat_hash_set
absl::node_hash_set
absl::memory
absl::synchronization
absl::optional
absl::span
)

target_link_libraries(scann_ops_cc_scann PUBLIC ${SOURCES})