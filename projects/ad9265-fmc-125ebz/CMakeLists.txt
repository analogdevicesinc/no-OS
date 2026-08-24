add_executable(ad9265-fmc-125ebz)

target_sources(ad9265-fmc-125ebz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad9265_fmc_125ebz.c
)

if(CONFIG_AD9265_FMC_125EBZ_IIO_EXAMPLE)
    target_compile_definitions(ad9265-fmc-125ebz PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad9265-fmc-125ebz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad9265-fmc-125ebz no-os)

config_platform_sdk(ad9265-fmc-125ebz)
add_flash_target(ad9265-fmc-125ebz)
post_build_config(ad9265-fmc-125ebz)
