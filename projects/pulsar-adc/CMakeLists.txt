add_executable(pulsar-adc)

# Common sources
target_sources(pulsar-adc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_PULSAR_ADC_BASIC_EXAMPLE)
    target_sources(pulsar-adc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(pulsar-adc PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
    target_compile_definitions(pulsar-adc PRIVATE BASIC_EXAMPLE)
endif()

# IIO example
if(CONFIG_PULSAR_ADC_IIO_EXAMPLE)
    target_sources(pulsar-adc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(pulsar-adc PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
    target_compile_definitions(pulsar-adc PRIVATE IIO_EXAMPLE)
endif()

target_include_directories(pulsar-adc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Platform-specific definitions
if(PLATFORM STREQUAL "xilinx")
    target_compile_definitions(pulsar-adc PRIVATE SPI_ENGINE_OFFLOAD_ENABLE)
elseif(PLATFORM STREQUAL "stm32")
    target_compile_definitions(pulsar-adc PRIVATE USE_STANDARD_SPI)
endif()

target_link_libraries(pulsar-adc no-os m)

config_platform_sdk(pulsar-adc)
add_flash_target(pulsar-adc)
post_build_config(pulsar-adc)
