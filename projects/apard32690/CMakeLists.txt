add_executable(apard32690)

# Common sources
target_sources(apard32690 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
)

# Basic example
if(CONFIG_APARD32690_BASIC_EXAMPLE)
    target_sources(apard32690 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(apard32690 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

# TCP echo server example
if(CONFIG_APARD32690_ECHO_SERVER_EXAMPLE)
    target_sources(apard32690 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/tcp_echo_server_example/tcp_echo_server_example.c
    )
    target_include_directories(apard32690 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/tcp_echo_server_example
    )
endif()

# ESH example
if(CONFIG_APARD32690_ESH_EXAMPLE)
    target_sources(apard32690 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/esh_example/esh_example.c
    )
    target_include_directories(apard32690 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/esh_example
    )
endif()

# ADIN1110 standalone example
if(CONFIG_APARD32690_ADIN1110_STANDALONE_EXAMPLE)
    target_sources(apard32690 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/adin1110_standalone_example/adin1110_standalone_example.c
    )
    target_include_directories(apard32690 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/adin1110_standalone_example
    )
endif()

target_include_directories(apard32690 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

target_link_libraries(apard32690 no-os)

config_platform_sdk(apard32690)
add_flash_target(apard32690)
post_build_config(apard32690)
