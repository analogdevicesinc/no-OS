add_executable(eval-ublox-gnss)

target_sources(eval-ublox-gnss PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interrupt/interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.c
)

target_include_directories(eval-ublox-gnss PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interrupt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ublox-gnss no-os)

config_platform_sdk(eval-ublox-gnss)
add_flash_target(eval-ublox-gnss)
post_build_config(eval-ublox-gnss)
