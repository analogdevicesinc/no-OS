# libiio.cmake - libiio's IIO daemon as a no-OS library (CONFIG_LIBIIO)
# Source tree resolved by resolve_library_source (override -> managed clone);
# libiio's no-OS/module.cmake then builds libiio-iiod and links it into the
# project.
include(LibraryCacheUtils)

message(STATUS "libiio requested version: ${CONFIG_LIBIIO_VERSION}")

resolve_library_source(
    libiio
    "${CONFIG_LIBIIO_VERSION}"
    "${NO_OS_DIR}/libraries/libiio"
    "https://github.com/analogdevicesinc/libiio"
    LIBIIO_SOURCE_DIR
    LIBIIO_BINARY_DIR
)

include("${LIBIIO_SOURCE_DIR}/no-OS/module.cmake")

message(STATUS "libiio configured from: ${LIBIIO_SOURCE_DIR}")
