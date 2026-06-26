# azure.cmake - Azure SDK for C with hybrid caching
# Priority: Submodule -> Global Cache -> FetchContent
include(LibraryCacheUtils)

if(NOT DEFINED CONFIG_AZURE_VERSION OR "${CONFIG_AZURE_VERSION}" STREQUAL "")
    set(CONFIG_AZURE_VERSION "1.5.0")
endif()

message(STATUS "Azure SDK requested version: ${CONFIG_AZURE_VERSION}")

resolve_library_source(
    azure_sdk
    "${CONFIG_AZURE_VERSION}"
    "${NO_OS_DIR}/libraries/azure/azure-sdk-for-c"
    "https://github.com/Azure/azure-sdk-for-c.git"
    AZURE_SOURCE_DIR
    AZURE_BINARY_DIR
)

target_include_directories(no-os PUBLIC
    "${AZURE_SOURCE_DIR}/sdk/inc"
)

message(STATUS "Azure SDK configured from: ${AZURE_SOURCE_DIR}")
