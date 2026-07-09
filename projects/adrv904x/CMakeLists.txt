add_executable(adrv904x)

target_sources(adrv904x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/clkgen_routines.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal/no_os_platform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/initdata.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_ADRV904X_BASIC_EXAMPLE)
    target_sources(adrv904x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(adrv904x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

if(CONFIG_ADRV904X_DMA_EXAMPLE)
    target_sources(adrv904x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma_example/dma_example.c
    )
    target_include_directories(adrv904x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma_example
    )
endif()

if(CONFIG_ADRV904X_IIO_EXAMPLE)
    target_sources(adrv904x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(adrv904x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(adrv904x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
)

# The koror driver (in the no-os library) includes project-supplied headers:
# no_os_platform.h (adrv904x_hal_cfg) from src/common/hal, and initdata.h
# (adrv904x_*_init defaults) from src/common. Both dirs must be on the no-os
# include path, since the driver is compiled into no-os, not this target.
target_include_directories(no-os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

config_platform_sdk(adrv904x)
add_flash_target(adrv904x)
post_build_config(adrv904x)
target_link_libraries(adrv904x no-os)
