# set(SCANN_SOURCE_DIR ${ClickHouse_SOURCE_DIR}/contrib/google-research/scann)

# set(SCANN_BUILD_DIR ${ClickHouse_SOURCE_DIR}/contrib/google-research/scann/build)
# set(SCANN_INCLUDE_DIRS ${SCANN_SOURCE_DIR}/scann_api)

# # add_library(scann_lib SHARED IMPORTED)
# # set_target_properties(scann_lib PROPERTIES IMPORTED_LOCATION
# # ${SCANN_BUILD_DIR}/libscann_interface.a)

# set(SCANN_LIBRARIES ${SCANN_BUILD_DIR}/scann_api/scann/libscann_interface.so)

# add_library(_scann INTERFACE)
# target_link_libraries(_scann INTERFACE ${SCANN_LIBRARIES})
# target_include_directories(_scann SYSTEM INTERFACE ${SCANN_INCLUDE_DIRS})

# add_library(ch_contrib::scann ALIAS _scann)