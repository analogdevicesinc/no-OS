add_executable(cn0565)

# Common sources (always compiled)
target_sources(cn0565 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

target_include_directories(cn0565 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
)

# Basic EIT (UART console) example
if(CONFIG_CN0565_EIT_EXAMPLE)
    target_sources(cn0565 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/eit/eit.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/eit/app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/eit/mux_board.c
    )
    target_include_directories(cn0565 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/eit
    )
endif()

# IIO EIT example
if(CONFIG_CN0565_EIT_IIO_EXAMPLE)
    target_sources(cn0565 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/eit_iio/eit_iio.c
    )
    target_include_directories(cn0565 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/eit_iio
    )
endif()

target_link_libraries(cn0565 no-os m)

config_platform_sdk(cn0565)
add_flash_target(cn0565)
post_build_config(cn0565)
