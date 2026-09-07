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

# Bench-specific strap selection. Which two pins are jumpered is a property of
# the setup on the desk, not of the board, so it is a build-time knob rather
# than an edit to parameters.h:
#
#   cmake ... -DGPIO_STRAP=5
#
# parameters.h guards every strap macro with #ifndef, so the individual
# pin/port macros can be overridden the same way for a pairing it does not
# name. Passing these through target_compile_definitions rather than
# CMAKE_C_FLAGS is deliberate: the Maxim toolchain file overwrites
# CMAKE_C_FLAGS, so a -D put there is silently dropped.
foreach(_strap_opt GPIO_STRAP
                   GPIO_OUTPUT_IDENTIFIER GPIO_OUTPUT_PIN GPIO_OUTPUT_NUM_PINS
                   GPIO_INPUT_IDENTIFIER  GPIO_INPUT_PIN  GPIO_INPUT_NUM_PINS)
    if(DEFINED ${_strap_opt})
        target_compile_definitions(capi_selftest PUBLIC
            ${_strap_opt}=${${_strap_opt}})
        message(STATUS "capi_selftest: strap override ${_strap_opt}=${${_strap_opt}}")
    endif()
endforeach()

target_link_libraries(capi_selftest no-os)

# BSP + flash go through the shared cmake/xilinx dispatchers for every platform.
config_platform_sdk(capi_selftest)
add_flash_target(capi_selftest)
post_build_config(capi_selftest)
