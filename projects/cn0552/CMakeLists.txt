add_executable(cn0552)

# The cn0552 project has a single-file src/app layout.
target_sources(cn0552 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/headless.c
)

target_include_directories(cn0552 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(cn0552 no-os)

config_platform_sdk(cn0552)
add_flash_target(cn0552)
post_build_config(cn0552)
