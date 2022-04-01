add_library(scann_interface SHARED
scann_builder.hpp
scann_builder.cpp
scann_ops_pybind.hpp
scann_ops_pybind.cpp
dataset.hpp
)

target_link_libraries(scann_interface PUBLIC scann_ops_cc_scann)
target_include_directories(scann_interface PUBLIC ${SCANN_COMMON_INCLUDE_DIRS})