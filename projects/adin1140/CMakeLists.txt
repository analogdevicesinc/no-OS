add_executable(adin1140)

# Common sources
target_sources(adin1140 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Frame RX/TX example
if(CONFIG_ADIN1140_FRAME_RX_TX_EXAMPLE)
    target_sources(adin1140 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/frame_rx_tx/frame_rx_tx_example.c
    )
    target_include_directories(adin1140 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/frame_rx_tx
    )
    target_compile_definitions(adin1140 PRIVATE EXAMPLE=frame_rx_tx)
endif()

# Raw L2 throughput (frame blast) example
if(CONFIG_ADIN1140_FRAME_BLAST_EXAMPLE)
    target_sources(adin1140 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/frame_blast/frame_blast_example.c
    )
    target_include_directories(adin1140 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/frame_blast
    )
    target_compile_definitions(adin1140 PRIVATE EXAMPLE=frame_blast)
endif()

# iperf over lwIP example
if(CONFIG_ADIN1140_IPERF_LWIP_EXAMPLE)
    target_sources(adin1140 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iperf_lwip_example/iperf_lwip_example.c
    )
    target_include_directories(adin1140 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iperf_lwip_example
    )
    target_compile_definitions(adin1140 PRIVATE EXAMPLE=iperf_lwip_example)
endif()

# iperf over lwIP on FreeRTOS example
if(CONFIG_ADIN1140_IPERF_LWIP_FREERTOS_EXAMPLE)
    target_sources(adin1140 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iperf_lwip_freertos/iperf_lwip_freertos.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iperf_lwip_freertos/iperf_menu.c
    )
    target_include_directories(adin1140 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iperf_lwip_freertos
    )
    target_compile_definitions(adin1140 PRIVATE EXAMPLE=iperf_lwip_freertos)
endif()

target_include_directories(adin1140 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Board macro derived from the selected STM32 board, consumed by the STM32
# platform sources to pick the board-specific HAL/clock/EXTI configuration.
if(BOARD STREQUAL "nucleo-h563zi")
    target_compile_definitions(adin1140 PRIVATE NUCLEO_H563ZI)
endif()

target_link_libraries(adin1140 no-os)

config_platform_sdk(adin1140)
add_flash_target(adin1140)
post_build_config(adin1140)
