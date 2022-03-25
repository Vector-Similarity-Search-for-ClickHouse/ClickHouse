add_library(utils_internal_avx_funcs INTERFACE)

set(SOURCES
utils_types
utils_intrinsics_avx1
)

target_link_libraries(utils_internal_avx_funcs INTERFACE ${SOURCES})
 

add_library(utils_internal_avx2_funcs INTERFACE)

set(SOURCES
utils_types
utils_intrinsics_avx2
)

target_link_libraries(utils_internal_avx2_funcs INTERFACE ${SOURCES})