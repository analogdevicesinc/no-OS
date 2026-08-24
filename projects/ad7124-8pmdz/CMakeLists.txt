add_executable(ad7124-8pmdz)

# Common sources
target_sources(ad7124-8pmdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/ad7124_regs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# IIO example
if(CONFIG_AD7124_8PMDZ_IIO_EXAMPLE)
    target_sources(ad7124-8pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(ad7124-8pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# Wi-Fi networking (ESP8266 AT-command transport, aducm3029/eval-adicup3029)
if(CONFIG_AD7124_8PMDZ_NETWORKING)
    target_sources(ad7124-8pmdz PRIVATE
        ${NO_OS_DIR}/network/tcp_socket.c
        ${NO_OS_DIR}/network/wifi/wifi.c
        ${NO_OS_DIR}/network/wifi/at_parser.c
    )
    target_include_directories(ad7124-8pmdz PUBLIC
        ${NO_OS_DIR}/network
        ${NO_OS_DIR}/network/wifi
    )
    target_compile_definitions(ad7124-8pmdz PRIVATE NO_OS_NETWORKING DISABLE_SECURE_SOCKET)
    target_include_directories(no-os PUBLIC
        ${NO_OS_DIR}/network
        ${NO_OS_DIR}/network/wifi
    )
    target_compile_definitions(no-os PUBLIC NO_OS_NETWORKING DISABLE_SECURE_SOCKET)
endif()

target_include_directories(ad7124-8pmdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad7124-8pmdz no-os)

config_platform_sdk(ad7124-8pmdz)
add_flash_target(ad7124-8pmdz)
post_build_config(ad7124-8pmdz)
