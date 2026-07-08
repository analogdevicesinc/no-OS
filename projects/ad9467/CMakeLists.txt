add_executable(ad9467)

target_sources(ad9467 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/ad9467_fmc.c
)

if(CONFIG_AD9467_IIO_EXAMPLE)
    target_compile_definitions(ad9467 PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad9467 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/adi_hal
)

target_link_libraries(ad9467 no-os)

config_platform_sdk(ad9467)
add_flash_target(ad9467)
post_build_config(ad9467)
