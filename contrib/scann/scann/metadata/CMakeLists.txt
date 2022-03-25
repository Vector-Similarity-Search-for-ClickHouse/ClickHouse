add_library(metadata_metadata_getter SHARED metadata_getter.cc metadata_getter.h)

set(SOURCES
data_format_datapoint
data_format_dataset
cc_proto
utils_types

absl::status
absl::synchronization
)

target_link_libraries(metadata_metadata_getter PUBLIC ${SOURCES})