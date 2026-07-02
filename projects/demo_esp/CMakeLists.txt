add_executable(demo_esp)

# Common sources
target_sources(demo_esp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_DEMO_ESP_BASIC_EXAMPLE)
    target_sources(demo_esp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
        ${NO_OS_DIR}/network/tcp_socket.c
        ${NO_OS_DIR}/network/wifi/wifi.c
        ${NO_OS_DIR}/network/wifi/at_parser.c
    )
    target_include_directories(demo_esp PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
        ${NO_OS_DIR}/network/wifi
    )
    target_compile_definitions(demo_esp PRIVATE
        DISABLE_SECURE_SOCKET
    )
    # Provide tcp_socket.h and network_interface.h for the mqtt noos support
    # layer compiled into no-os. These are not yet gated in network/CMakeLists.txt
    # on anything other than CONFIG_LWIP; this project uses wifi AT transport instead.
    target_include_directories(no-os PUBLIC
        ${NO_OS_DIR}/network
    )
    target_compile_definitions(no-os PUBLIC
        DISABLE_SECURE_SOCKET
    )
endif()

target_include_directories(demo_esp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(demo_esp no-os)

config_platform_sdk(demo_esp)
add_flash_target(demo_esp)
post_build_config(demo_esp)
