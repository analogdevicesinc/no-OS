add_executable(ad74416h)

# Common sources
target_sources(ad74416h PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_AD74416H_BASIC_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# Current input ext example
if(CONFIG_AD74416H_CURRENT_INPUT_EXT_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_input_ext/current_input_ext.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_input_ext
    )
endif()

# Current input loop example
if(CONFIG_AD74416H_CURRENT_INPUT_LOOP_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_input_loop/current_input_loop.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_input_loop
    )
endif()

# Current output example
if(CONFIG_AD74416H_CURRENT_OUTPUT_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_output/current_output.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_output
    )
endif()

# Digital input logic example
if(CONFIG_AD74416H_DIGITAL_INPUT_LOGIC_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_input_logic/digital_input_logic.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_input_logic
    )
endif()

# Digital input loop example
if(CONFIG_AD74416H_DIGITAL_INPUT_LOOP_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_input_loop/digital_input_loop.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_input_loop
    )
endif()

# Digital output example
if(CONFIG_AD74416H_DIGITAL_OUTPUT_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_output/digital_output.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_output
    )
endif()

# Multiple devices example
if(CONFIG_AD74416H_MULTIPLE_DEVICES_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/multiple_devices/multiple_devices.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/multiple_devices
    )
endif()

# Temperature 2wire rtd example
if(CONFIG_AD74416H_TEMPERATURE_2WIRE_RTD_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/temperature_2wire_rtd/temperature_2wire_rtd.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/temperature_2wire_rtd
    )
endif()

# Temperature 3wire rtd example
if(CONFIG_AD74416H_TEMPERATURE_3WIRE_RTD_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/temperature_3wire_rtd/temperature_3wire_rtd.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/temperature_3wire_rtd
    )
endif()

# Voltage input example
if(CONFIG_AD74416H_VOLTAGE_INPUT_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/voltage_input/voltage_input.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/voltage_input
    )
endif()

# Voltage input irq example
if(CONFIG_AD74416H_VOLTAGE_INPUT_IRQ_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/voltage_input_irq/voltage_input_irq.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/voltage_input_irq
    )
endif()

# Voltage output example
if(CONFIG_AD74416H_VOLTAGE_OUTPUT_EXAMPLE)
    target_sources(ad74416h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/voltage_output/voltage_output.c
    )
    target_include_directories(ad74416h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/voltage_output
    )
endif()

target_include_directories(ad74416h PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad74416h no-os)

config_platform_sdk(ad74416h)
add_flash_target(ad74416h)
post_build_config(ad74416h)
