add_executable(hello_world)

target_sources(hello_world PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_HELLO_WORLD_BASIC_EXAMPLE)
    target_sources(hello_world PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(hello_world PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

target_include_directories(hello_world PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(hello_world no-os)

config_platform_sdk(hello_world)
add_flash_target(hello_world)
post_build_config(hello_world)
