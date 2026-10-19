add_executable(apardpfwd)

# Common sources
target_sources(apardpfwd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
)

# APARD communication example
if(CONFIG_APARDPFWD_APARD_COMMUNICATION_EXAMPLE)
    target_sources(apardpfwd PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/apard_communication_example/apard_communication_example.c
    )
    target_include_directories(apardpfwd PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/apard_communication_example
    )
    target_compile_definitions(apardpfwd PRIVATE NO_OS_STATIC_IP)
endif()

# Forward packets example
if(CONFIG_APARDPFWD_FORWARD_PACKETS_EXAMPLE)
    target_sources(apardpfwd PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/forward_packets_example/forward_packets_example.c
    )
    target_include_directories(apardpfwd PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/forward_packets_example
    )
endif()

# Communication and forward example
if(CONFIG_APARDPFWD_COMMUNICATION_AND_FORWARD_EXAMPLE)
    target_sources(apardpfwd PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/communication_and_forward_example/communication_and_forward_example.c
    )
    target_include_directories(apardpfwd PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/communication_and_forward_example
    )
    target_compile_definitions(apardpfwd PRIVATE NO_OS_STATIC_IP)
endif()

# Servo control example
if(CONFIG_APARDPFWD_SERVO_CONTROL_EXAMPLE)
    target_sources(apardpfwd PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/apard_servo_control/apard_servo_control.c
    )
    target_include_directories(apardpfwd PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/apard_servo_control
    )
    target_compile_definitions(apardpfwd PRIVATE NO_OS_STATIC_IP)
endif()

target_include_directories(apardpfwd PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(apardpfwd no-os)

config_platform_sdk(apardpfwd)
add_flash_target(apardpfwd)
post_build_config(apardpfwd)
