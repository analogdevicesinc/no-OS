add_executable(max78000)

# Common sources
target_sources(max78000 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
)

# Basic example
if(CONFIG_MAX78000_BASIC_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

target_include_directories(max78000 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(max78000 no-os)

config_platform_sdk(max78000)
add_flash_target(max78000)
post_build_config(max78000)
