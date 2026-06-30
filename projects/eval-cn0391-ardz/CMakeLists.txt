add_executable(eval-cn0391-ardz)

# Common sources (always compiled)
target_sources(eval-cn0391-ardz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/cn0391.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# The cn0391 driver uses the thermocouple and RTD temperature utilities,
# which are not registered in the shared util/CMakeLists.txt.
target_sources(eval-cn0391-ardz PRIVATE
    ${CMAKE_SOURCE_DIR}/util/temperature/no_os_thermocouple.c
    ${CMAKE_SOURCE_DIR}/util/temperature/no_os_rtd.c
)

# Basic example
if(CONFIG_EVAL_CN0391_ARDZ_BASIC_EXAMPLE)
    target_sources(eval-cn0391-ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(eval-cn0391-ardz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO LWIP example
if(CONFIG_EVAL_CN0391_ARDZ_IIO_LWIP_EXAMPLE)
    target_sources(eval-cn0391-ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_lwip_example/iio_lwip_example.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/iio_cn0391.c
    )
    target_include_directories(eval-cn0391-ardz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_lwip_example
    )
endif()

target_include_directories(eval-cn0391-ardz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Link libm: the RTD temperature conversion (no_os_rtd.c) uses sqrt().
target_link_libraries(eval-cn0391-ardz no-os m)

config_platform_sdk(eval-cn0391-ardz)
add_flash_target(eval-cn0391-ardz)
post_build_config(eval-cn0391-ardz)
