add_executable(wethlink)

target_sources(wethlink PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/led.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/mwc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/net.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_WETHLINK_WETHLINK_REVA_EXAMPLE)
    target_compile_definitions(wethlink PRIVATE HW_VERSION=0)
endif()

if(CONFIG_WETHLINK_WETHLINK_REVB_EXAMPLE)
    target_compile_definitions(wethlink PRIVATE HW_VERSION=1)
endif()

target_include_directories(wethlink PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(wethlink no-os)

config_platform_sdk(wethlink)
add_flash_target(wethlink)
post_build_config(wethlink)
