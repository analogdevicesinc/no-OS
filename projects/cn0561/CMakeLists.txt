add_executable(cn0561)

target_sources(cn0561 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cn0561.c
)

if(CONFIG_CN0561_IIO_EXAMPLE)
    target_compile_definitions(cn0561 PRIVATE IIO_SUPPORT)
endif()

if(CONFIG_CN0561_ZED_CARRIER)
    target_compile_definitions(cn0561 PRIVATE CN0561_ZED_CARRIER)
endif()

if(CONFIG_CN0561_CORAZ7S_CARRIER)
    target_compile_definitions(cn0561 PRIVATE CN0561_CORAZ7S_CARRIER)
endif()

target_include_directories(cn0561 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cn0561 no-os)

config_platform_sdk(cn0561)
add_flash_target(cn0561)
post_build_config(cn0561)
