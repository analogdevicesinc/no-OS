add_executable(ad9172)

target_sources(ad9172 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

# Bare #ifdef selectors in main.c — emit compile definitions per variant
if(CONFIG_AD9172_IIO_EXAMPLE)
    target_compile_definitions(ad9172 PRIVATE IIO_SUPPORT)
endif()

if(CONFIG_AD9172_DMA_EXAMPLE)
    target_compile_definitions(ad9172 PRIVATE DMA_EXAMPLE)
endif()

target_include_directories(ad9172 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

config_platform_sdk(ad9172)
add_flash_target(ad9172)
post_build_config(ad9172)
target_link_libraries(ad9172 no-os)
