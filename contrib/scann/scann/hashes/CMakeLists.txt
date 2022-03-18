add_subdirectory(asymmetric_hashing2)
add_subdirectory(internal)

add_library(hashes_hashing_base SHARED hashing_base.cc hashing_base.h)

set(SOURCES
data_format_datapoint
data_format_dataset
proto_hashed_cc_proto
utils_types
)

target_link_libraries(hashes_hashing_base PUBLIC ${SOURCES})