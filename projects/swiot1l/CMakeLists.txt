add_executable(swiot1l)

# Common sources
target_sources(swiot1l PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Default IIO firmware
if(CONFIG_SWIOT1L_DEFAULT_FW)
    target_sources(swiot1l PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/swiot_fw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/swiot.c
    )
    target_compile_definitions(swiot1l PRIVATE SWIOT1L_DEFAULT_FW)
    target_include_directories(swiot1l PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
endif()

# MQTT example
if(CONFIG_SWIOT1L_MQTT_EXAMPLE)
    target_sources(swiot1l PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/swiot1l-mqtt/swiot1l_mqtt.c
    )
    target_compile_definitions(swiot1l PRIVATE
        SWIOT1L_MQTT_EXAMPLE
        SWIOT1L_MQTT_SERVER_IP="${CONFIG_SWIOT1L_MQTT_SERVER_IP}"
        SWIOT1L_MQTT_SERVER_PORT=${CONFIG_SWIOT1L_MQTT_SERVER_PORT}
    )
    target_include_directories(swiot1l PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/swiot1l-mqtt
    )
endif()

# OA TC6 SPI protocol flag
if(CONFIG_SWIOT1L_OA_TC6_SPI)
    target_compile_definitions(swiot1l PRIVATE SWIOT1L_OA_TC6_SPI=1)
else()
    target_compile_definitions(swiot1l PRIVATE SWIOT1L_OA_TC6_SPI=0)
endif()

target_compile_definitions(swiot1l PRIVATE
    NO_OS_LWIP_NETWORKING
    IIO_IGNORE_BUFF_OVERRUN_ERR
)

target_include_directories(swiot1l PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(swiot1l no-os)

config_platform_sdk(swiot1l)
add_flash_target(swiot1l)
post_build_config(swiot1l)
