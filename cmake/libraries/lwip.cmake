# lwip.cmake - lwIP library with hybrid caching
# Priority: Submodule → Global Cache → FetchContent
include(LibraryCacheUtils)

# Default version if not set via Kconfig
if(NOT DEFINED CONFIG_LWIP_VERSION OR "${CONFIG_LWIP_VERSION}" STREQUAL "")
    set(CONFIG_LWIP_VERSION "STABLE-2_2_1_RELEASE")
endif()

message(STATUS "lwIP requested version: ${CONFIG_LWIP_VERSION}")

# Resolve library source using hybrid priority chain
resolve_library_source(
    lwip
    "${CONFIG_LWIP_VERSION}"
    "${NO_OS_DIR}/libraries/lwip/lwip"
    "https://github.com/lwip-tcpip/lwip"
    LWIP_SOURCE_DIR
    LWIP_BINARY_DIR
)

# If we already used FetchContent, lwip targets exist
# Otherwise, we need to add the subdirectory
if(NOT TARGET lwipcore)
    add_subdirectory("${LWIP_SOURCE_DIR}" "${LWIP_BINARY_DIR}" EXCLUDE_FROM_ALL)
endif()

# Set include directories based on resolved source
set(LWIP_INCLUDE_DIRS
    "${LWIP_SOURCE_DIR}/src/include"
    "${LWIP_SOURCE_DIR}/contrib"
    "${LWIP_SOURCE_DIR}/contrib/examples/example_app"
)

target_include_directories(lwipcore PUBLIC ${NO_OS_DIR}/libraries/lwip/configs)
target_include_directories(lwipcore PUBLIC ${NO_OS_DIR}/libraries/lwip)
target_include_directories(lwipcore PUBLIC ${LWIP_INCLUDE_DIRS})

target_include_directories(lwipallapps PRIVATE ${NO_OS_DIR}/libraries/lwip/configs)
target_include_directories(lwipallapps PRIVATE ${NO_OS_DIR}/libraries/lwip)
target_include_directories(lwipallapps PRIVATE ${LWIP_INCLUDE_DIRS})

target_compile_definitions(no-os PUBLIC -DNO_OS_LWIP_NETWORKING=1)
target_compile_definitions(no-os PUBLIC -DDISABLE_SECURE_SOCKET=1)

target_link_libraries(no-os PUBLIC lwipallapps lwipcore)

message(STATUS "lwIP configured from: ${LWIP_SOURCE_DIR}")
