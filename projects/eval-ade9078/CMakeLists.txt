add_executable(eval-ade9078)

target_sources(eval-ade9078 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.c
)

if(CONFIG_EVAL_ADE9078_ADE9078_EXAMPLE)
    target_include_directories(eval-ade9078 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    )
endif()

target_include_directories(eval-ade9078 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(eval-ade9078 no-os)

config_platform_sdk(eval-ade9078)
add_flash_target(eval-ade9078)
post_build_config(eval-ade9078)
