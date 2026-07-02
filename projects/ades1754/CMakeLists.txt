add_executable(ades1754)

# Common sources
target_sources(ades1754 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_ADES1754_BASIC_EXAMPLE)
    target_sources(ades1754 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ades1754 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# SPI UART bridge example
if(CONFIG_ADES1754_SPI_UART_BRIDGE_EXAMPLE)
    target_sources(ades1754 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/spi_uart_bridge/spi_uart_bridge_example.c
    )
    target_include_directories(ades1754 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/spi_uart_bridge
    )
endif()

target_include_directories(ades1754 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ades1754 no-os)

config_platform_sdk(ades1754)
add_flash_target(ades1754)
post_build_config(ades1754)
