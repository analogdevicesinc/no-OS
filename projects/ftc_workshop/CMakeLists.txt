add_executable(ftc_workshop ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

config_platform_sdk(ftc_workshop)
add_flash_target(ftc_workshop)
post_build_config(ftc_workshop)

target_include_directories(ftc_workshop PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(ftc_workshop no-os)