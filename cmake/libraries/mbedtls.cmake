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

# mbedtls 2.28.0 declares cmake_minimum_required(VERSION 2.8.12); CMake >= 4.0
# rejects compatibility with CMake < 3.5. Allow the old floor so the vendored
# tree still configures.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

if(NOT TARGET mbedtls)
    add_subdirectory("${MBEDTLS_SOURCE_DIR}" "${MBEDTLS_BINARY_DIR}" EXCLUDE_FROM_ALL)
endif()

# A project may supply its own mbedTLS configuration header (the legacy Makefile
# MBED_TLS_CONFIG_FILE convention, which added -DMBEDTLS_CONFIG_FILE to the global
# CFLAGS). Wire it up as MBEDTLS_CONFIG_FILE so mbedTLS headers include it instead
# of the default include/mbedtls/config.h. The define must reach the mbedTLS
# library sources (e.g. entropy_poll.c), no-os, and the project's own sources that
# include mbedtls headers (e.g. iot_example.c), so apply it to the mbedTLS targets
# PUBLIC and to no-os PUBLIC (all project executables link no-os).
if(CONFIG_MBEDTLS_CONFIG_FILE AND NOT "${CONFIG_MBEDTLS_CONFIG_FILE}" STREQUAL "")
    if("${CMAKE_SOURCE_DIR}" STREQUAL "${NO_OS_DIR}")
        set(_mbedtls_conf_abs "${NO_OS_DIR}/${CONFIG_MBEDTLS_CONFIG_FILE}")
    else()
        set(_mbedtls_conf_abs "${CMAKE_SOURCE_DIR}/${CONFIG_MBEDTLS_CONFIG_FILE}")
    endif()
    foreach(_mbedtls_tgt mbedcrypto mbedx509 mbedtls)
        target_compile_definitions(${_mbedtls_tgt} PUBLIC
            MBEDTLS_CONFIG_FILE="${_mbedtls_conf_abs}")
    endforeach()
    target_compile_definitions(no-os PUBLIC MBEDTLS_CONFIG_FILE="${_mbedtls_conf_abs}")
    message(STATUS "mbedTLS config file: ${_mbedtls_conf_abs}")
elseif(CMAKE_CROSSCOMPILING)
    # No project config: the stock config.h enables platform entropy sources,
    # whose entropy_poll.c #errors on bare-metal targets. Disable it on the
    # library's own sources (PUBLIC so dependents agree on the ABI).
    foreach(_mbedtls_tgt mbedcrypto mbedx509 mbedtls)
        target_compile_definitions(${_mbedtls_tgt} PUBLIC MBEDTLS_NO_PLATFORM_ENTROPY)
    endforeach()
endif()

target_link_libraries(no-os mbedtls mbedx509 mbedcrypto)

message(STATUS "mbedTLS configured from: ${MBEDTLS_SOURCE_DIR}")
