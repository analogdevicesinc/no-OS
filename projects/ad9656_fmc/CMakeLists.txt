add_executable(ad9656_fmc)

target_sources(ad9656_fmc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/ad9656_fmc.c
)

if(CONFIG_AD9656_FMC_IIO_EXAMPLE)
    target_compile_definitions(ad9656_fmc PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad9656_fmc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/adi_hal
)

config_platform_sdk(ad9656_fmc)
add_flash_target(ad9656_fmc)
post_build_config(ad9656_fmc)
target_link_libraries(ad9656_fmc no-os)
