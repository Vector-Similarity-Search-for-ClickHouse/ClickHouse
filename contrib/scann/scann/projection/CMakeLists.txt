
add_library(projection_projection_base SHARED projection_base.cc projection_base.h )

set(SOURCES
data_format_datapoint
data_format_dataset
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_tf_dependency
utils_types
utils_util_functions

absl::base
absl::flags
)

target_link_libraries(projection_projection_base PUBLIC ${SOURCES})
 

add_library(projection_chunking_projection SHARED chunking_projection.cc chunking_projection.h )

set(SOURCES
projection_identity_projection
projection_projection_base
data_format_datapoint
proto_projection_cc_proto
utils_datapoint_utils
utils_types

absl::strings
)

target_link_libraries(projection_chunking_projection PUBLIC ${SOURCES})
 

add_library(projection_random_orthogonal_projection SHARED random_orthogonal_projection.cc random_orthogonal_projection.h )

set(SOURCES
projection_projection_base
data_format_datapoint
oss_wrappers_scann_random
oss_wrappers_tf_dependency
utils_datapoint_utils
utils_types

absl::memory
absl::random_random
)

target_link_libraries(projection_random_orthogonal_projection PUBLIC ${SOURCES})
 

add_library(projection_identity_projection SHARED identity_projection.cc identity_projection.h )

set(SOURCES
projection_projection_base
data_format_datapoint
utils_types
)

target_link_libraries(projection_identity_projection PUBLIC ${SOURCES})
 

add_library(projection_projection_factory SHARED projection_factory.cc projection_factory.h )

set(SOURCES
projection_random_orthogonal_projection
projection_chunking_projection
projection_identity_projection
projection_projection_base
data_format_dataset
proto_projection_cc_proto
utils_types
)

target_link_libraries(projection_projection_factory PUBLIC ${SOURCES})