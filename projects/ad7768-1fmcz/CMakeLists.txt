add_executable(ad7768-1fmcz)

target_sources(ad7768-1fmcz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad77681evb.c
)

if(CONFIG_AD7768_1FMCZ_SPI_ENGINE_OFFLOAD_EXAMPLE)
    target_compile_definitions(ad7768-1fmcz PRIVATE SPI_ENGINE_OFFLOAD_EXAMPLE=1)
endif()

if(CONFIG_AD7768_1FMCZ_DEMO_EXAMPLE)
    target_compile_definitions(ad7768-1fmcz PRIVATE SPI_ENGINE_OFFLOAD_EXAMPLE=0)
endif()

target_include_directories(ad7768-1fmcz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad7768-1fmcz no-os)

config_platform_sdk(ad7768-1fmcz)
add_flash_target(ad7768-1fmcz)
post_build_config(ad7768-1fmcz)
