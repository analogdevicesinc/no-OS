add_executable(ad5460)

# Common sources
target_sources(ad5460 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_AD5460_BASIC_EXAMPLE)
    target_sources(ad5460 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad5460 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# Channel output example
if(CONFIG_AD5460_CHANNEL_OUTPUT_EXAMPLE)
    target_sources(ad5460 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/channel_output_example/channel_output_example.c
    )
    target_include_directories(ad5460 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/channel_output_example
    )
endif()

# Current output example
if(CONFIG_AD5460_CURRENT_OUTPUT_EXAMPLE)
    target_sources(ad5460 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_output_example/current_output_example.c
    )
    target_include_directories(ad5460 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_output_example
    )
endif()

# Voltage output example
if(CONFIG_AD5460_VOLTAGE_OUTPUT_EXAMPLE)
    target_sources(ad5460 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/voltage_output_example/voltage_output_example.c
    )
    target_include_directories(ad5460 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/voltage_output_example
    )
endif()

target_include_directories(ad5460 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad5460 no-os)

config_platform_sdk(ad5460)
add_flash_target(ad5460)
post_build_config(ad5460)
