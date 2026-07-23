add_executable(adiol100)

# Common sources
target_sources(adiol100 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_ADIOL100_BASIC_EXAMPLE)
    target_sources(adiol100 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(adiol100 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# i-link IO-Link stack example (FreeRTOS)
if(CONFIG_ADIOL100_ILINK_EXAMPLE)
    # Example source + port layer
    target_sources(adiol100 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/ilink/ilink_example.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/ilink/adiol100_ilink_pl.c
    )
    target_include_directories(adiol100 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/ilink
    )

    # i-link stack sources
    set(ILINK_DIR ${NO_OS_DIR}/libraries/ilink)
    target_sources(adiol100 PRIVATE
        ${ILINK_DIR}/src/iolink_main.c
        ${ILINK_DIR}/src/iolink_al.c
        ${ILINK_DIR}/src/iolink_cm.c
        ${ILINK_DIR}/src/iolink_dl.c
        ${ILINK_DIR}/src/iolink_ds.c
        ${ILINK_DIR}/src/iolink_sm.c
        ${ILINK_DIR}/src/iolink_pl.c
        ${ILINK_DIR}/src/iolink_ode.c
        ${ILINK_DIR}/src/iolink_pde.c
    )

    # i-link app handler
    target_sources(adiol100 PRIVATE
        ${ILINK_DIR}/app/app_handler.c
        ${ILINK_DIR}/app/app_smi.c
        ${ILINK_DIR}/app/app_device.c
    )

    # OSAL FreeRTOS implementation
    target_sources(adiol100 PRIVATE
        ${ILINK_DIR}/osal/osal_freertos.c
    )

    # i-link include paths
    target_include_directories(adiol100 PUBLIC
        ${ILINK_DIR}
        ${ILINK_DIR}/include
        ${ILINK_DIR}/src
        ${ILINK_DIR}/osal
        ${ILINK_DIR}/app
    )

    target_link_libraries(adiol100 freertos_kernel)
endif()

target_include_directories(adiol100 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(adiol100 no-os)

config_platform_sdk(adiol100)
add_flash_target(adiol100)
post_build_config(adiol100)
