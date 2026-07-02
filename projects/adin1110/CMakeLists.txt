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

target_include_directories(adin1110 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(adin1110 no-os)

config_platform_sdk(adin1110)
add_flash_target(adin1110)
post_build_config(adin1110)
