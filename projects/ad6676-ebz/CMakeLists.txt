add_executable(ad6676-ebz)

target_sources(ad6676-ebz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad6676_ebz.c
)

if(CONFIG_AD6676_EBZ_IIO_EXAMPLE)
    target_compile_definitions(ad6676-ebz PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad6676-ebz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad6676-ebz no-os)

config_platform_sdk(ad6676-ebz)
add_flash_target(ad6676-ebz)
post_build_config(ad6676-ebz)
