add_executable(eval-adl8113)

# Common sources
target_sources(eval-adl8113 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_EVAL_ADL8113_BASIC_EXAMPLE)
    target_sources(eval-adl8113 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(eval-adl8113 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_EVAL_ADL8113_IIO_EXAMPLE)
    target_sources(eval-adl8113 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(eval-adl8113 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(eval-adl8113 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-adl8113 no-os)

config_platform_sdk(eval-adl8113)
add_flash_target(eval-adl8113)
post_build_config(eval-adl8113)
