add_executable(ada4250_ardz)

# Common sources
target_sources(ada4250_ardz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# IIO example
if(CONFIG_ADA4250_ARDZ_IIO_EXAMPLE)
    target_sources(ada4250_ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(ada4250_ardz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(ada4250_ardz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ada4250_ardz no-os)

config_platform_sdk(ada4250_ardz)
add_flash_target(ada4250_ardz)
post_build_config(ada4250_ardz)
