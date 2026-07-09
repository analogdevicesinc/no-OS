add_executable(ad9434-fmc-500ebz)

target_sources(ad9434-fmc-500ebz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad9434_fmc_500ebz.c
)

if(CONFIG_AD9434_FMC_500EBZ_IIO_EXAMPLE)
    target_compile_definitions(ad9434-fmc-500ebz PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad9434-fmc-500ebz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad9434-fmc-500ebz no-os)

config_platform_sdk(ad9434-fmc-500ebz)
add_flash_target(ad9434-fmc-500ebz)
post_build_config(ad9434-fmc-500ebz)
