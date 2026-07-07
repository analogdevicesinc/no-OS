add_executable(ad7746-ebz)

# The ad7746-ebz project has a single-file src/app layout; the IIO code path
# is compiled in when CONFIG_IIO is enabled.
target_sources(ad7746-ebz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/headless.c
)

target_include_directories(ad7746-ebz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

if(CONFIG_IIO)
    target_compile_definitions(ad7746-ebz PRIVATE IIO_SUPPORT)
endif()

target_link_libraries(ad7746-ebz no-os)

config_platform_sdk(ad7746-ebz)
add_flash_target(ad7746-ebz)
post_build_config(ad7746-ebz)
