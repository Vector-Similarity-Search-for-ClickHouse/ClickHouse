add_library(utils_intrinsics_simd INTERFACE)

set(SOURCES
utils_intrinsics_attributes
utils_intrinsics_avx1
utils_intrinsics_avx2
utils_intrinsics_avx512
utils_intrinsics_fallback
utils_intrinsics_flags
utils_intrinsics_sse4
)

target_link_libraries(utils_intrinsics_simd INTERFACE ${SOURCES})
 

add_library(utils_intrinsics_horizontal_sum INTERFACE)

set(SOURCES
utils_intrinsics_simd
)

target_link_libraries(utils_intrinsics_horizontal_sum INTERFACE ${SOURCES})
 

add_library(utils_intrinsics_flags SHARED flags.cc flags.h )

set(SOURCES
oss_wrappers_scann_aligned_malloc
oss_wrappers_scann_down_cast
oss_wrappers_tf_dependency
utils_types

absl::base
absl::flags
)

target_link_libraries(utils_intrinsics_flags PUBLIC ${SOURCES})
 

add_library(utils_intrinsics_attributes INTERFACE)
 

add_library(utils_intrinsics_fallback INTERFACE)

set(SOURCES
utils_intrinsics_attributes
utils_intrinsics_flags
utils_index_sequence
utils_types
)

target_link_libraries(utils_intrinsics_fallback INTERFACE ${SOURCES})
 

add_library(utils_intrinsics_sse4 INTERFACE)

set(SOURCES
utils_intrinsics_attributes
utils_intrinsics_flags
utils_index_sequence
utils_types
)

target_link_libraries(utils_intrinsics_sse4 INTERFACE ${SOURCES})
 

add_library(utils_intrinsics_avx1 INTERFACE)

set(SOURCES
utils_intrinsics_attributes
utils_intrinsics_flags
utils_intrinsics_sse4
utils_index_sequence
utils_types
)

target_link_libraries(utils_intrinsics_avx1 INTERFACE ${SOURCES})
 

add_library(utils_intrinsics_avx2 INTERFACE)

set(SOURCES
utils_intrinsics_attributes
utils_intrinsics_avx1
utils_intrinsics_flags
utils_index_sequence
utils_types
)

target_link_libraries(utils_intrinsics_avx2 INTERFACE ${SOURCES})
 

add_library(utils_intrinsics_avx512 INTERFACE)

set(SOURCES
utils_intrinsics_attributes
utils_intrinsics_avx2
utils_intrinsics_flags
utils_index_sequence
utils_types
)

target_link_libraries(utils_intrinsics_avx512 INTERFACE ${SOURCES})
