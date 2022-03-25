add_library(oss_wrappers_tf_dependency INTERFACE )

set(SOURCES
${TensorFlow_LIBRARIES}
)

target_link_libraries(oss_wrappers_tf_dependency INTERFACE ${SOURCES})
 

add_library(oss_wrappers_scann_aligned_malloc SHARED scann_aligned_malloc.cc scann_aligned_malloc.h )


add_library(oss_wrappers_scann_down_cast INTERFACE)

set(SOURCES
absl::base
)

target_link_libraries(oss_wrappers_scann_down_cast INTERFACE ${SOURCES})
 

add_library(oss_wrappers_scann_castops INTERFACE)


add_library(oss_wrappers_scann_bits SHARED scann_bits.cc scann_bits.h )

set(SOURCES
absl::core_headers
absl::bits
)

target_link_libraries(oss_wrappers_scann_bits PUBLIC ${SOURCES})
 

add_library(oss_wrappers_scann_malloc_extension INTERFACE)

set(SOURCES
absl::strings
absl::optional
)

target_link_libraries(oss_wrappers_scann_malloc_extension INTERFACE ${SOURCES})
 

add_library(oss_wrappers_scann_random INTERFACE)
 

add_library(oss_wrappers_scann_status SHARED scann_status.cc scann_status.h )

set(SOURCES
oss_wrappers_scann_status_builder
oss_wrappers_tf_dependency

absl::strings
)

target_link_libraries(oss_wrappers_scann_status PUBLIC ${SOURCES})
 

add_library(oss_wrappers_scann_status_builder SHARED scann_status_builder.cc scann_status_builder.h )

set(SOURCES
oss_wrappers_tf_dependency

absl::memory
absl::strings
)

target_link_libraries(oss_wrappers_scann_status_builder PUBLIC ${SOURCES})
 

add_library(oss_wrappers_scann_serialize SHARED scann_serialize.cc scann_serialize.h )

set(SOURCES

absl::base
absl::endian
absl::strings
)

target_link_libraries(oss_wrappers_scann_serialize PUBLIC ${SOURCES})
 

add_library(oss_wrappers_scann_threadpool INTERFACE)

set(SOURCES
oss_wrappers_tf_dependency
)

target_link_libraries(oss_wrappers_scann_threadpool INTERFACE ${SOURCES})
 