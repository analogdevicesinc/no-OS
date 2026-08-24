add_executable(ad719x)

target_sources(ad719x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_AD719X_DUMMY_EXAMPLE)
    target_sources(ad719x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy/dummy_example.c
    )
    target_include_directories(ad719x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy
    )
    target_compile_definitions(ad719x PRIVATE DUMMY_EXAMPLE)
endif()

target_include_directories(ad719x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad719x no-os)

config_platform_sdk(ad719x)
add_flash_target(ad719x)
post_build_config(ad719x)
