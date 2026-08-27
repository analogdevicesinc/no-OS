# Source tree resolved by resolve_library_source (override -> managed clone).
include(LibraryCacheUtils)

resolve_library_source(
    ilink
    "${CONFIG_ILINK_VERSION}"
    "${NO_OS_DIR}/libraries/ilink"
    "https://github.com/rtlabs-com/i-link.git"
    ILINK_SOURCE_DIR
    ILINK_BINARY_DIR
)

message(STATUS "i-link configured from: ${ILINK_SOURCE_DIR}")
