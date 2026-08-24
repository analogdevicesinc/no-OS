add_executable(ad7768-evb)

target_sources(ad7768-evb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad7768_evb.c
)

if(CONFIG_AD7768_EVB_IIO_EXAMPLE)
    target_compile_definitions(ad7768-evb PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad7768-evb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad7768-evb no-os)

config_platform_sdk(ad7768-evb)
add_flash_target(ad7768-evb)
post_build_config(ad7768-evb)
