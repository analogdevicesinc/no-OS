# mqtt.cmake - Paho MQTT Embedded-C library with hybrid caching
# Priority: Submodule -> Global Cache -> FetchContent
#
# The paho CMakeLists.txt builds shared libraries not suitable for embedded,
# so we compile the required source files directly.
include(LibraryCacheUtils)

if(NOT DEFINED CONFIG_MQTT_VERSION OR "${CONFIG_MQTT_VERSION}" STREQUAL "")
    set(CONFIG_MQTT_VERSION "master")
endif()

message(STATUS "MQTT requested version: ${CONFIG_MQTT_VERSION}")

resolve_library_source(
    paho_mqtt
    "${CONFIG_MQTT_VERSION}"
    "${NO_OS_DIR}/libraries/mqtt/paho.mqtt.embedded-c"
    "https://github.com/eclipse/paho.mqtt.embedded-c"
    PAHO_SOURCE_DIR
    PAHO_BINARY_DIR
)

set(PAHO_PACKET_DIR "${PAHO_SOURCE_DIR}/MQTTPacket/src")
set(PAHO_CLIENT_DIR "${PAHO_SOURCE_DIR}/MQTTClient-C/src")
set(MQTT_WRAPPER_DIR "${NO_OS_DIR}/libraries/mqtt")

target_sources(no-os PRIVATE
    ${MQTT_WRAPPER_DIR}/mqtt_client.c
    ${MQTT_WRAPPER_DIR}/mqtt_noos_support.c
    ${PAHO_PACKET_DIR}/MQTTConnectClient.c
    ${PAHO_PACKET_DIR}/MQTTDeserializePublish.c
    ${PAHO_PACKET_DIR}/MQTTFormat.c
    ${PAHO_PACKET_DIR}/MQTTPacket.c
    ${PAHO_PACKET_DIR}/MQTTSerializePublish.c
    ${PAHO_PACKET_DIR}/MQTTSubscribeClient.c
    ${PAHO_PACKET_DIR}/MQTTUnsubscribeClient.c
    ${PAHO_CLIENT_DIR}/MQTTClient.c
)

target_include_directories(no-os PUBLIC
    ${MQTT_WRAPPER_DIR}
    ${PAHO_CLIENT_DIR}
    ${PAHO_PACKET_DIR}
)

target_compile_definitions(no-os PUBLIC
    MQTTCLIENT_PLATFORM_HEADER=mqtt_noos_support.h
)

# Paho sources are missing #include <string.h> for strcmp/memset/etc.
# GCC 14+ treats implicit function declarations as errors.
# Use SHELL: so "-include string.h" is passed as two separate arguments rather
# than collapsed into one token (which leaves a bare "string.h" on the command line).
target_compile_options(no-os PRIVATE "$<$<COMPILE_LANGUAGE:C>:SHELL:-include string.h>")

message(STATUS "MQTT configured from: ${PAHO_SOURCE_DIR}")
