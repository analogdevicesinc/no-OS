add_executable(ad4692_ardz)

# Common sources
target_sources(ad4692_ardz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_AD4692_ARDZ_BASIC_EXAMPLE)
    target_sources(ad4692_ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad4692_ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_AD4692_ARDZ_IIO_EXAMPLE)
    target_sources(ad4692_ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(ad4692_ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

# IIO trigger example
if(CONFIG_AD4692_ARDZ_IIO_TRIGGER_EXAMPLE)
    target_sources(ad4692_ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger/iio_trigger_example.c
    )
    target_include_directories(ad4692_ardz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger
    )
    target_compile_definitions(ad4692_ardz PRIVATE IIO_IGNORE_BUFF_OVERRUN_ERR)
endif()

target_include_directories(ad4692_ardz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad4692_ardz no-os)

config_platform_sdk(ad4692_ardz)
add_flash_target(ad4692_ardz)
post_build_config(ad4692_ardz)
