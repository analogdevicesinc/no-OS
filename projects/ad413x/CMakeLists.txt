add_executable(ad413x)

target_sources(ad413x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.c
)

if(CONFIG_AD413X_IIO_EXAMPLE)
    target_compile_definitions(ad413x PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad413x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(ad413x no-os)

config_platform_sdk(ad413x)
add_flash_target(ad413x)
post_build_config(ad413x)
