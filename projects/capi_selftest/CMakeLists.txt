add_executable(capi_selftest)

target_sources(capi_selftest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/test_framework.c
)

if(PLATFORM STREQUAL "stm32" AND CONFIG_CAPI_I2C)
	target_sources(capi_selftest PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/stm32/i2c_platform_init.c
	)
endif()

# SPI DMA bring-up hook. The file is internally guarded by
# #ifdef SPI_DMA_PLATFORM_INIT, so it compiles to nothing on boards whose
# parameters.h leaves the SPI DMA path disabled; it provides
# spi_dma_platform_init() when that macro is defined (see test_spi.c ASYNC_DMA).
if(PLATFORM STREQUAL "stm32" AND CONFIG_CAPI_SPI)
	target_sources(capi_selftest PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/stm32/spi_dma_platform_init.c
	)
endif()

if(PLATFORM STREQUAL "stm32" AND CONFIG_CAPI_SPI)
	target_sources(capi_selftest PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/stm32/spi_platform_init.c
	)
endif()

if(CONFIG_CAPI_SELFTEST_BASIC_EXAMPLE)
    target_sources(capi_selftest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(capi_selftest PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

if(CONFIG_CAPI_SELFTEST_LOOPBACK_EXAMPLE)
    target_sources(capi_selftest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback/capi_loopback.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback/tests/test_gpio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback/tests/test_irq.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback/tests/test_spi.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback/tests/test_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback/tests/test_i2c.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback/tests/test_uart.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback/tests/test_dma.c
    )
    target_include_directories(capi_selftest PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/capi_loopback
    )
endif()

target_include_directories(capi_selftest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

target_link_libraries(capi_selftest no-os)

# BSP + flash go through the shared cmake/xilinx dispatchers for every platform.
config_platform_sdk(capi_selftest)
add_flash_target(capi_selftest)
post_build_config(capi_selftest)
