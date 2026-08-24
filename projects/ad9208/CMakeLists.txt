add_executable(ad9208)

target_sources(ad9208 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

if(CONFIG_AD9208_IIO_EXAMPLE)
    target_compile_definitions(ad9208 PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad9208 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

config_platform_sdk(ad9208)
add_flash_target(ad9208)
post_build_config(ad9208)
target_link_libraries(ad9208 no-os)
