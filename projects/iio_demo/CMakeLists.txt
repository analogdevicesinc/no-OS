add_executable(iio_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

config_platform_sdk(iio_demo)
add_flash_target(iio_demo)

target_include_directories(iio_demo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(iio_demo no-os)

target_compile_definitions(iio_demo PRIVATE
    -DIIO_SUPPORT=1
    -DIIO_EXAMPLE=1
)

add_custom_command(
    TARGET iio_demo
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:iio_demo> iio_demo.hex
    COMMENT "Generating iio_demo.hex"
)