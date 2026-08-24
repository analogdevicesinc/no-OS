add_executable(ad9081)

target_sources(ad9081 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_clock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_jesd.c
)

if(CONFIG_AD9081_IIO_EXAMPLE)
    target_compile_definitions(ad9081 PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad9081 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/profiles/vcu118_ad9081_m8_l4
)

# The project-provided app_config.h is included by app_clock.h, app.c, and
# app_jesd.c. These compile into the project target and the include path above
# covers them. No no-os PUBLIC path is needed (unlike ad9083 where a driver
# wrapper directly included a project header).

config_platform_sdk(ad9081)
add_flash_target(ad9081)
post_build_config(ad9081)
target_link_libraries(ad9081 no-os)
