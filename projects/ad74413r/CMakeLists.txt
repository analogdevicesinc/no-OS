add_executable(ad74413r
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy/dummy_example.c
)

config_platform_sdk(ad74413r)
add_flash_target(ad74413r)
post_build_config(ad74413r)

target_include_directories(ad74413r PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy
)

target_link_libraries(ad74413r no-os)

add_custom_command(
    TARGET ad74413r
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:ad74413r> ad74413r.hex
    COMMENT "Generating ad74413r.hex"
)