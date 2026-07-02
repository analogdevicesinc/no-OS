add_executable(eval-ad7490sdz)

# Common sources
target_sources(eval-ad7490sdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_EVAL_AD7490SDZ_BASIC_EXAMPLE)
    target_sources(eval-ad7490sdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(eval-ad7490sdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_EVAL_AD7490SDZ_IIO_EXAMPLE)
    target_sources(eval-ad7490sdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(eval-ad7490sdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(eval-ad7490sdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ad7490sdz no-os)

config_platform_sdk(eval-ad7490sdz)
add_flash_target(eval-ad7490sdz)
post_build_config(eval-ad7490sdz)
