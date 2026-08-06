add_executable(adrv903x)

target_sources(adrv903x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/clkgen_routines.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal/no_os_platform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(DEFINED XILINX_ARCH AND "${XILINX_ARCH}" MATCHES "cortexa72")
    target_sources(adrv903x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/versal/initdata.c
    )
    target_include_directories(adrv903x BEFORE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/versal
    )
else()
    target_sources(adrv903x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/initdata.c
    )
endif()

if(CONFIG_ADRV903X_BASIC_EXAMPLE)
    target_sources(adrv903x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(adrv903x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

if(CONFIG_ADRV903X_DMA_EXAMPLE)
    target_sources(adrv903x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma_example/dma_example.c
    )
    target_include_directories(adrv903x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma_example
    )
endif()

if(CONFIG_ADRV903X_IIO_EXAMPLE)
    target_sources(adrv903x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(adrv903x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(adrv903x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
)

target_include_directories(no-os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal
)

target_compile_definitions(no-os PUBLIC ADI_ADRV903X_API_VERSION_RANGE_CHECK=0)

target_link_options(adrv903x PRIVATE -Wl,--wrap=fseek -Wl,--wrap=fclose)
target_link_options(adrv903x PRIVATE -Xlinker --defsym=_HEAP_SIZE=0x800000)


config_platform_sdk(adrv903x)
add_flash_target(adrv903x)
post_build_config(adrv903x)
target_link_libraries(adrv903x no-os)
target_link_libraries(adrv903x m)
