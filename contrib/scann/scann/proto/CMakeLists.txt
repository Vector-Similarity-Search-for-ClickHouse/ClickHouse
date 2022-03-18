set(PROTO_SOURCES
scann.proto
brute_force.proto
crowding.proto
disjoint_restrict_token.proto
metadata.proto
restricts.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_scann_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_scann_cc_proto PUBLIC
proto_partitioning_cc_proto
proto_distance_measure_cc_proto
proto_exact_reordering_cc_proto
proto_input_output_cc_proto
proto_hash_cc_proto)


set(PROTO_SOURCES
crowding.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_crowding_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})


set(PROTO_SOURCES
input_output.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_input_output_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_input_output_cc_proto PUBLIC
proto_incremental_updates_cc_proto)


set(PROTO_SOURCES
restricts.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_restricts_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})


set(PROTO_SOURCES
hash.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_hash_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_hash_cc_proto PUBLIC
proto_distance_measure_cc_proto
proto_projection_cc_proto
)


set(PROTO_SOURCES
metadata.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_metadata_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_metadata_cc_proto PUBLIC
proto_distance_measure_cc_proto
)


set(PROTO_SOURCES
brute_force.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_brute_force_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_brute_force_cc_proto PUBLIC
proto_exact_reordering_cc_proto
)


set(PROTO_SOURCES
exact_reordering.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_exact_reordering_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_exact_reordering_cc_proto PUBLIC
proto_distance_measure_cc_proto
)


set(PROTO_SOURCES
partitioning.proto
${Protobuf_IMPORT_DIRS}/duration.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_partitioning_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_partitioning_cc_proto PUBLIC
proto_distance_measure_cc_proto
proto_exact_reordering_cc_proto
proto_input_output_cc_proto
proto_projection_cc_proto
)


set(PROTO_SOURCES
projection.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_projection_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})


set(PROTO_SOURCES
distance_measure.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_distance_measure_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})


set(PROTO_SOURCES
results.proto
../data_format/features.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_results_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})


set(PROTO_SOURCES
centers.proto
../data_format/features.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_centers_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_centers_cc_proto PUBLIC
proto_hash_cc_proto)


set(PROTO_SOURCES
hashed.proto
../data_format/features.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_hashed_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})


set(PROTO_SOURCES
incremental_updates.proto
${Protobuf_IMPORT_DIRS}/duration.proto
${Protobuf_IMPORT_DIRS}/timestamp.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCES})

add_library(proto_incremental_updates_cc_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})

