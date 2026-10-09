add_executable(hello_world)

target_sources(hello_world PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(PLATFORM STREQUAL "altera")
    target_compile_definitions(hello_world PRIVATE ALTERA_PLATFORM)
    target_sources(hello_world PRIVATE
        ${NO_OS_DIR}/drivers/platform/altera/altera_delay.c
    )
    target_include_directories(hello_world PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/altera
        ${CMAKE_SOURCE_DIR}/include
    )
    set_target_properties(hello_world PROPERTIES OUTPUT_NAME "hello_world.elf")
endif()

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

if(NOT PLATFORM STREQUAL "altera")
    target_link_libraries(hello_world no-os)
endif()

config_platform_sdk(hello_world)
add_flash_target(hello_world)
post_build_config(hello_world)
