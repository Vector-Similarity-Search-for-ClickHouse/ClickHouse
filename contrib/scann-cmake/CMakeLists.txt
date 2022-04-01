#set(SCANN_ROOT_DIR "${ClickHouse_SOURCE_DIR}/contrib/scann")

add_subdirectory("${ClickHouse_SOURCE_DIR}/contrib/scann" "${ClickHouse_BINARY_DIR}/contrib/scann")

add_library(_scann INTERFACE)
target_link_libraries(_scann INTERFACE scann_interface)
target_include_directories(_scann SYSTEM INTERFACE ${SCANN_INCLUDE_DIRS})

add_library(ch_contrib::scann ALIAS _scann)
