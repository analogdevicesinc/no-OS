# tmc.cmake - TMC-API sources with hybrid caching
# Source tree resolved by resolve_library_source (override -> managed clone).
#
# TMC-API has no usable CMakeLists.txt; the required source is compiled directly
# into no-os. This module only resolves the source tree and exposes TMC_SOURCE_DIR.
include(LibraryCacheUtils)

if(NOT DEFINED CONFIG_TMC_API_VERSION OR "${CONFIG_TMC_API_VERSION}" STREQUAL "")
    set(CONFIG_TMC_API_VERSION "3.11.4")
endif()

message(STATUS "TMC-API requested version: ${CONFIG_TMC_API_VERSION}")

resolve_library_source(
    tmc
    "${CONFIG_TMC_API_VERSION}"
    "${NO_OS_DIR}/libraries/TMC-API"
    "https://github.com/analogdevicesinc/TMC-API.git"
    TMC_SOURCE_DIR
    TMC_BINARY_DIR
)

message(STATUS "TMC-API configured from: ${TMC_SOURCE_DIR}")
