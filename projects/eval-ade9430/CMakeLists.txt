add_executable(eval-ade9430)

target_sources(eval-ade9430 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
)

if(CONFIG_EVAL_ADE9430_BASIC_EXAMPLE)
    target_sources(eval-ade9430 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(eval-ade9430 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
    if(CONFIG_EVAL_ADE9430_HW_RESET)
        target_compile_definitions(eval-ade9430 PRIVATE HW_RESET_ENABLED)
    endif()
endif()

if(CONFIG_EVAL_ADE9430_IOT_EXAMPLE)
    target_sources(eval-ade9430 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iot_example/iot_example.c
        ${NO_OS_DIR}/network/wifi/wifi.c
        ${NO_OS_DIR}/network/wifi/at_parser.c
        ${NO_OS_DIR}/network/tcp_socket.c
        ${NO_OS_DIR}/drivers/display/nhd_c12832a1z/nhd_c12832a1z.c
    )
    target_include_directories(eval-ade9430 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iot_example
        ${NO_OS_DIR}/network/wifi
        ${NO_OS_DIR}/network
        ${NO_OS_DIR}/drivers/display/nhd_c12832a1z
    )
    # tcp_socket.h is included by mqtt headers but not exposed by mqtt.cmake
    target_include_directories(no-os PUBLIC ${NO_OS_DIR}/network)
    if(CONFIG_EVAL_ADE9430_IOT_AZURE)
        target_sources(eval-ade9430 PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iot_example/iot_sample_common.c
        )
        target_compile_definitions(eval-ade9430 PRIVATE AZURE_IOT_HUB=1)
    else()
        # DISABLE_SECURE_SOCKET must also reach the no-os target: tcp_socket.c
        # and the mqtt noos support layer are compiled into no-os and gate
        # their mbedtls/ssl.h include on this define.
        target_compile_definitions(eval-ade9430 PRIVATE DISABLE_SECURE_SOCKET)
        target_compile_definitions(no-os PUBLIC DISABLE_SECURE_SOCKET)
    endif()
    if(CONFIG_EVAL_ADE9430_IOT_RTC)
        target_compile_definitions(eval-ade9430 PRIVATE RTC_SET_DEFAULT)
    endif()
    if(CONFIG_EVAL_ADE9430_IOT_DPS)
        target_compile_definitions(eval-ade9430 PRIVATE CONFIG_DPS)
    endif()
endif()

target_include_directories(eval-ade9430 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ade9430 no-os)

config_platform_sdk(eval-ade9430)
add_flash_target(eval-ade9430)
post_build_config(eval-ade9430)
