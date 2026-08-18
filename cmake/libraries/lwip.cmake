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

# lwipopts.h comes from CONFIG_LWIP_CONF_PATH when a project/board defconfig sets it,
# shadowing the generic libraries/lwip-glue/configs/lwipopts.h. The generic dir must stay on
# the path regardless because it also provides lwipcfg.h (used by
# network/lwip_raw_socket/lwip_socket.c), so this is NOT either/or.
if(CONFIG_LWIP_CONF_PATH AND NOT "${CONFIG_LWIP_CONF_PATH}" STREQUAL "")
    if(IS_ABSOLUTE "${CONFIG_LWIP_CONF_PATH}")
        set(_lwip_conf_dir "${CONFIG_LWIP_CONF_PATH}")
    elseif("${CMAKE_SOURCE_DIR}" STREQUAL "${NO_OS_DIR}")
        set(_lwip_conf_dir "${NO_OS_DIR}/${CONFIG_LWIP_CONF_PATH}")
    else()
        set(_lwip_conf_dir "${CMAKE_SOURCE_DIR}/${CONFIG_LWIP_CONF_PATH}")
    endif()

    # CONFIG_LWIP_CONF_PATH may point directly at lwipopts.h; the include search
    # needs the directory that contains it.
    if(EXISTS "${_lwip_conf_dir}" AND NOT IS_DIRECTORY "${_lwip_conf_dir}")
        get_filename_component(_lwip_conf_dir "${_lwip_conf_dir}" DIRECTORY)
    endif()

    target_include_directories(lwipcore SYSTEM PUBLIC ${_lwip_conf_dir})
    target_include_directories(lwipallapps SYSTEM PRIVATE ${_lwip_conf_dir})
    target_include_directories(no-os SYSTEM PUBLIC ${_lwip_conf_dir})
    message(STATUS "lwipopts.h: ${_lwip_conf_dir}/lwipopts.h")
endif()

target_include_directories(lwipcore SYSTEM PUBLIC
    ${NO_OS_DIR}/libraries/lwip-glue/configs
    ${NO_OS_DIR}/libraries/lwip-glue
    ${LWIP_INCLUDE_DIRS})

target_include_directories(lwipallapps SYSTEM PRIVATE
    ${NO_OS_DIR}/libraries/lwip-glue/configs
    ${NO_OS_DIR}/libraries/lwip-glue
    ${LWIP_INCLUDE_DIRS})

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
