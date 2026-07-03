add_executable(ssd1306)

target_sources(ssd1306 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_SSD1306_EXAMPLE_EXAMPLE)
    target_sources(ssd1306 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example/example.c
    )
    target_include_directories(ssd1306 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example
    )
    target_compile_definitions(ssd1306 PRIVATE SSD_1306_EXAMPLE)
endif()

target_include_directories(ssd1306 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ssd1306 no-os lvgl)

config_platform_sdk(ssd1306)
add_flash_target(ssd1306)
post_build_config(ssd1306)
