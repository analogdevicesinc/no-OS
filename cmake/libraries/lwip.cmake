# lwip.cmake - lwIP library with hybrid caching
# Source tree resolved by resolve_library_source (override -> managed clone).
include(LibraryCacheUtils)

message(STATUS "lwIP requested version: ${CONFIG_LWIP_VERSION}")

# Resolve library source using hybrid priority chain
resolve_library_source(
    lwip
    "${CONFIG_LWIP_VERSION}"
    "${NO_OS_DIR}/libraries/lwip"
    "https://github.com/lwip-tcpip/lwip"
    LWIP_SOURCE_DIR
    LWIP_BINARY_DIR
)

# If a prior include added lwip targets, reuse them
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

target_include_directories(lwipcore PUBLIC ${NO_OS_DIR}/libraries/lwip-glue/configs)
target_include_directories(lwipcore PUBLIC ${NO_OS_DIR}/libraries/lwip-glue)
target_include_directories(lwipcore PUBLIC ${LWIP_INCLUDE_DIRS})

target_include_directories(lwipallapps PRIVATE ${NO_OS_DIR}/libraries/lwip-glue/configs)
target_include_directories(lwipallapps PRIVATE ${NO_OS_DIR}/libraries/lwip-glue)
target_include_directories(lwipallapps PRIVATE ${LWIP_INCLUDE_DIRS})

target_compile_definitions(no-os PUBLIC -DNO_OS_LWIP_NETWORKING=1)
target_compile_definitions(no-os PUBLIC -DDISABLE_SECURE_SOCKET=1)

target_link_libraries(no-os PUBLIC lwipallapps lwipcore)

# Wire in FreeRTOS sys_arch port when FreeRTOS is also enabled.
# lwipcore/lwipallapps are compiled without -include no_os_config.h, so the
# CONFIG_FREERTOS macro that lwipopts.h keys NO_SYS off is defined here to keep
# lwipcore and no-os consistent (NO_SYS==0 with FreeRTOS, NO_SYS==1 otherwise).
if(CONFIG_FREERTOS)
    target_compile_definitions(lwipcore PUBLIC CONFIG_FREERTOS=1)
    target_compile_definitions(lwipallapps PUBLIC CONFIG_FREERTOS=1)
    target_sources(lwipcore PRIVATE
        ${LWIP_SOURCE_DIR}/contrib/ports/freertos/sys_arch.c
    )
    # Both lwipcore and lwipallapps pull in lwip/sys.h, which includes
    # arch/sys_arch.h under NO_SYS==0, so the port include dir is needed on both.
    target_include_directories(lwipcore PUBLIC
        ${LWIP_SOURCE_DIR}/contrib/ports/freertos/include
    )
    target_include_directories(lwipallapps PUBLIC
        ${LWIP_SOURCE_DIR}/contrib/ports/freertos/include
    )
    target_link_libraries(lwipcore PUBLIC freertos_kernel)
endif()

message(STATUS "lwIP configured from: ${LWIP_SOURCE_DIR}")
