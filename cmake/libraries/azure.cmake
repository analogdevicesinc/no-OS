# azure.cmake - Azure SDK for C with hybrid caching
# Source tree resolved by resolve_library_source (override -> managed clone).
include(LibraryCacheUtils)

if(NOT DEFINED CONFIG_AZURE_VERSION OR "${CONFIG_AZURE_VERSION}" STREQUAL "")
    set(CONFIG_AZURE_VERSION "1.5.0")
endif()

message(STATUS "Azure SDK requested version: ${CONFIG_AZURE_VERSION}")

resolve_library_source(
    azure_sdk
    "${CONFIG_AZURE_VERSION}"
    "${NO_OS_DIR}/libraries/azure-sdk-for-c"
    "https://github.com/Azure/azure-sdk-for-c.git"
    AZURE_SOURCE_DIR
    AZURE_BINARY_DIR
)

# Header directories. Sources include azure headers by bare name (e.g.
# az_iot_hub_client.h), so expose the subdirs that hold them, not just sdk/inc.
target_include_directories(no-os PUBLIC
    "${AZURE_SOURCE_DIR}/sdk/inc"
    "${AZURE_SOURCE_DIR}/sdk/inc/azure"
    "${AZURE_SOURCE_DIR}/sdk/inc/azure/core"
    "${AZURE_SOURCE_DIR}/sdk/inc/azure/iot"
)

# Compile the SDK sources directly into no-os instead of invoking the SDK's own
# CMake (which needs an older CMake policy and a separate toolchain file). This
# mirrors the legacy Makefile lib set: az_core + az_iot_* + the embedded
# no-platform / no-http stubs (posix/win32/curl are host-only and excluded).
file(GLOB _az_core_srcs "${AZURE_SOURCE_DIR}/sdk/src/azure/core/*.c")
file(GLOB _az_iot_srcs "${AZURE_SOURCE_DIR}/sdk/src/azure/iot/*.c")
target_sources(no-os PRIVATE
    ${_az_core_srcs}
    ${_az_iot_srcs}
    ${AZURE_SOURCE_DIR}/sdk/src/azure/platform/az_noplatform.c
    ${AZURE_SOURCE_DIR}/sdk/src/azure/platform/az_nohttp.c
)

message(STATUS "Azure SDK configured from: ${AZURE_SOURCE_DIR}")
