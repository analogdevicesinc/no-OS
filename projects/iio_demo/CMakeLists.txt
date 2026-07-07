add_executable(iio_demo)

# Common sources
target_sources(iio_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic IIO example
if(CONFIG_IIO_DEMO_IIO_EXAMPLE)
    target_sources(iio_demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(iio_demo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# IIO timer trigger example
if(CONFIG_IIO_DEMO_IIO_TIMER_TRIGGER_EXAMPLE)
    target_sources(iio_demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.c
    )
    target_include_directories(iio_demo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_timer_trigger_example
    )
endif()

# IIO software trigger example
if(CONFIG_IIO_DEMO_IIO_SW_TRIGGER_EXAMPLE)
    target_sources(iio_demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_sw_trigger_example/iio_sw_trigger_example.c
    )
    target_include_directories(iio_demo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_sw_trigger_example
    )
endif()

# ESP8266 Wi-Fi networking (aducm3029/eval-adicup3029)
if(CONFIG_IIO_DEMO_NETWORKING)
    target_sources(iio_demo PRIVATE
        ${NO_OS_DIR}/network/tcp_socket.c
        ${NO_OS_DIR}/network/wifi/wifi.c
        ${NO_OS_DIR}/network/wifi/at_parser.c
    )
    target_include_directories(iio_demo PUBLIC
        ${NO_OS_DIR}/network
        ${NO_OS_DIR}/network/wifi
    )
    target_compile_definitions(iio_demo PRIVATE NO_OS_NETWORKING DISABLE_SECURE_SOCKET)
    # iio_app.c is compiled into no-os and switches on NO_OS_NETWORKING, so the
    # define and the network headers must reach that target too.
    # DISABLE_SECURE_SOCKET selects the non-mbedTLS tcp_socket path.
    target_include_directories(no-os PUBLIC
        ${NO_OS_DIR}/network
        ${NO_OS_DIR}/network/wifi
    )
    target_compile_definitions(no-os PUBLIC NO_OS_NETWORKING DISABLE_SECURE_SOCKET)
endif()

target_include_directories(iio_demo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(iio_demo no-os)

config_platform_sdk(iio_demo)
add_flash_target(iio_demo)
post_build_config(iio_demo)
