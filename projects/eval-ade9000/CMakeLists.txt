add_executable(eval-ade9000)

# Common sources
target_sources(eval-ade9000 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.c
)

# ADE9000 example
if(CONFIG_EVAL_ADE9000_ADE9000_EXAMPLE)
    target_include_directories(eval-ade9000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    )
endif()

target_include_directories(eval-ade9000 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ade9000 no-os)

config_platform_sdk(eval-ade9000)
add_flash_target(eval-ade9000)
post_build_config(eval-ade9000)
