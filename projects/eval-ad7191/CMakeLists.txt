add_executable(eval-ad7191)

# Common sources
target_sources(eval-ad7191 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_EVAL_AD7191_BASIC_EXAMPLE)
    target_sources(eval-ad7191 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(eval-ad7191 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

# IIO example
if(CONFIG_EVAL_AD7191_IIO_EXAMPLE)
    target_sources(eval-ad7191 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(eval-ad7191 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )

    # On Linux the IIO transport is served over TCP (iiod) using the host
    # network stack.
    if(PLATFORM STREQUAL "linux")
        target_sources(eval-ad7191 PRIVATE
            ${NO_OS_DIR}/network/tcp_socket.c
            ${NO_OS_DIR}/network/linux_socket/linux_socket.c
        )
        target_include_directories(no-os PUBLIC
            ${NO_OS_DIR}/network
            ${NO_OS_DIR}/network/linux_socket
        )
        target_compile_definitions(no-os PUBLIC DISABLE_SECURE_SOCKET)
    endif()
endif()

target_include_directories(eval-ad7191 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ad7191 no-os)

config_platform_sdk(eval-ad7191)
add_flash_target(eval-ad7191)
post_build_config(eval-ad7191)
