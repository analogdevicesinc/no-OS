add_executable(eval-ade7754)

target_sources(eval-ade7754 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interrupt/interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/platform.c
)

target_include_directories(eval-ade7754 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interrupt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ade7754 no-os)

config_platform_sdk(eval-ade7754)
add_flash_target(eval-ade7754)
post_build_config(eval-ade7754)
