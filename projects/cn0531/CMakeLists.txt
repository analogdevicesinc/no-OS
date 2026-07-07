add_executable(cn0531)

# Common sources
target_sources(cn0531 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# IIO example
if(CONFIG_CN0531_IIO_EXAMPLE)
    target_sources(cn0531 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(cn0531 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# ESP8266 Wi-Fi networking (aducm3029/eval-adicup3029)
if(CONFIG_CN0531_NETWORKING)
    target_sources(cn0531 PRIVATE
        ${NO_OS_DIR}/network/tcp_socket.c
        ${NO_OS_DIR}/network/wifi/wifi.c
        ${NO_OS_DIR}/network/wifi/at_parser.c
    )
    target_include_directories(cn0531 PUBLIC
        ${NO_OS_DIR}/network
        ${NO_OS_DIR}/network/wifi
    )
    target_compile_definitions(cn0531 PRIVATE NO_OS_NETWORKING DISABLE_SECURE_SOCKET)
    # iio_app.c is compiled into no-os and switches on NO_OS_NETWORKING, so the
    # define and the network headers must reach that target too.
    # DISABLE_SECURE_SOCKET selects the non-mbedTLS tcp_socket path.
    target_include_directories(no-os PUBLIC
        ${NO_OS_DIR}/network
        ${NO_OS_DIR}/network/wifi
    )
    target_compile_definitions(no-os PUBLIC NO_OS_NETWORKING DISABLE_SECURE_SOCKET)
endif()

target_include_directories(cn0531 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(cn0531 no-os)

config_platform_sdk(cn0531)
add_flash_target(cn0531)
post_build_config(cn0531)
