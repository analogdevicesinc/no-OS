# mbedtls.cmake - mbedTLS library with hybrid caching
# Priority: Submodule -> Global Cache -> FetchContent
include(LibraryCacheUtils)

if(NOT DEFINED CONFIG_MBEDTLS_VERSION OR "${CONFIG_MBEDTLS_VERSION}" STREQUAL "")
    set(CONFIG_MBEDTLS_VERSION "mbedtls-2.28.0")
endif()

message(STATUS "mbedTLS requested version: ${CONFIG_MBEDTLS_VERSION}")

resolve_library_source(
    mbedtls
    "${CONFIG_MBEDTLS_VERSION}"
    "${NO_OS_DIR}/libraries/mbedtls"
    "https://github.com/ARMmbed/mbedtls"
    MBEDTLS_SOURCE_DIR
    MBEDTLS_BINARY_DIR
)

# Disable components not needed for embedded use
set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)

if(NOT TARGET mbedtls)
    add_subdirectory("${MBEDTLS_SOURCE_DIR}" "${MBEDTLS_BINARY_DIR}" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(no-os mbedtls mbedx509 mbedcrypto)

message(STATUS "mbedTLS configured from: ${MBEDTLS_SOURCE_DIR}")
