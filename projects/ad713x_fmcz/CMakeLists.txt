add_executable(ad713x_fmcz)

target_sources(ad713x_fmcz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad713x_fmc.c
)

if(CONFIG_AD713X_FMCZ_IIO_EXAMPLE)
    target_compile_definitions(ad713x_fmcz PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad713x_fmcz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad713x_fmcz no-os)

config_platform_sdk(ad713x_fmcz)
add_flash_target(ad713x_fmcz)
post_build_config(ad713x_fmcz)
