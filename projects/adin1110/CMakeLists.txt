add_executable(adin1110)

# Common sources
target_sources(adin1110 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Frame RX/TX example
if(CONFIG_ADIN1110_FRAME_RX_TX_EXAMPLE)
    target_sources(adin1110 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/frame_rx_tx/frame_rx_tx_example.c
    )
    target_include_directories(adin1110 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/frame_rx_tx
    )
endif()

# ADIN1110 OA TC6 full duplex performance example
if(CONFIG_ADIN1110_OA_TC6_PERF_EXAMPLE)
    target_sources(adin1110 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/oa_tc6_perf/oa_tc6_perf_example.c
    )
    target_include_directories(adin1110 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/oa_tc6_perf
    )
endif()

# ADIN1110 lwIP UDP throughput example
if(CONFIG_ADIN1110_LWIP_UDP_PERF_EXAMPLE)
    target_sources(adin1110 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/lwip_udp_perf/lwip_udp_perf_example.c
    )
    target_include_directories(adin1110 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/lwip_udp_perf
    )
endif()

target_include_directories(adin1110 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(adin1110 no-os)

config_platform_sdk(adin1110)
add_flash_target(adin1110)
post_build_config(adin1110)
