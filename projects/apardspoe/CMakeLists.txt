add_executable(apardspoe)

# Common sources
target_sources(apardspoe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
)

# APARD communication example
if(CONFIG_APARDSPOE_APARD_COMMUNICATION_EXAMPLE)
    target_sources(apardspoe PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/apard_communication_example/apard_communication_example.c
    )
    target_include_directories(apardspoe PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/apard_communication_example
    )
    target_compile_definitions(apardspoe PRIVATE NO_OS_STATIC_IP)
endif()

# Color sensor example
if(CONFIG_APARDSPOE_COLOR_SENSOR_EXAMPLE)
    target_sources(apardspoe PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/color_sensor_example/color_sensor_example.c
    )
    target_include_directories(apardspoe PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/color_sensor_example
    )
    target_compile_definitions(apardspoe PRIVATE NO_OS_STATIC_IP)
endif()

target_include_directories(apardspoe PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(apardspoe no-os)

config_platform_sdk(apardspoe)
add_flash_target(apardspoe)
post_build_config(apardspoe)
