add_executable(demo_esp)

# Common sources
target_sources(demo_esp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_DEMO_ESP_BASIC_EXAMPLE)
    target_sources(demo_esp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(demo_esp PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
    target_compile_definitions(no-os PUBLIC
        DISABLE_SECURE_SOCKET
    )
endif()

target_include_directories(demo_esp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(demo_esp no-os)

config_platform_sdk(demo_esp)
add_flash_target(demo_esp)
post_build_config(demo_esp)
