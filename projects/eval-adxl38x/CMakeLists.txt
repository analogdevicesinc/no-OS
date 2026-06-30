add_executable(eval-adxl38x)

# Common sources
target_sources(eval-adxl38x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_EVAL_ADXL38X_BASIC_EXAMPLE)
    target_sources(eval-adxl38x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example_main.c
    )
    target_include_directories(eval-adxl38x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# Selftest example
if(CONFIG_EVAL_ADXL38X_SELFTEST_EXAMPLE)
    target_sources(eval-adxl38x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/selftest/selftest_example_main.c
    )
    target_include_directories(eval-adxl38x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/selftest
    )
endif()

# FIFO example
if(CONFIG_EVAL_ADXL38X_FIFO_EXAMPLE)
    target_sources(eval-adxl38x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/fifo/fifo_example_main.c
    )
    target_include_directories(eval-adxl38x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/fifo
    )
endif()

target_include_directories(eval-adxl38x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Select the target device based on platform.
# STM32 builds use ADXL382, Maxim builds use ADXL380.
if(PLATFORM STREQUAL "stm32")
    target_compile_definitions(eval-adxl38x PRIVATE ADXL382_DEV=1)
else()
    target_compile_definitions(eval-adxl38x PRIVATE ADXL380_DEV=1)
endif()

target_link_libraries(eval-adxl38x no-os)

config_platform_sdk(eval-adxl38x)
add_flash_target(eval-adxl38x)
post_build_config(eval-adxl38x)
