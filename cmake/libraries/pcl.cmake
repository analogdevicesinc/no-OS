# pcl.cmake - precision-converters-library sources with hybrid caching
# Source tree resolved by resolve_library_source (override -> managed clone).
#
# Consumers compile individual sources from this tree directly and add its
# subdirs as include paths. This module resolves the tree and exposes PCL_SOURCE_DIR.
include(LibraryCacheUtils)

message(STATUS "precision-converters-library requested version: ${CONFIG_PCL_VERSION}")

resolve_library_source(
    pcl
    "${CONFIG_PCL_VERSION}"
    "${NO_OS_DIR}/libraries/precision-converters-library"
    "https://github.com/analogdevicesinc/precision-converters-library.git"
    PCL_SOURCE_DIR
    PCL_BINARY_DIR
)

message(STATUS "precision-converters-library configured from: ${PCL_SOURCE_DIR}")
