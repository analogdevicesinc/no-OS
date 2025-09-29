add_executable(ftc_workshop
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main_mcs_style.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mcs_app_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stack_mcs_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services/svc_mcs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/profiles/mcs/mcs_main.c
)

config_platform_sdk(ftc_workshop)
add_flash_target(ftc_workshop)
post_build_config(ftc_workshop)

target_include_directories(ftc_workshop PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/profiles/mcs
)
target_link_libraries(ftc_workshop no-os)