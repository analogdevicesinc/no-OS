add_executable(max14906)

# Common sources
target_sources(max14906 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_MAX14906_BASIC_EXAMPLE)
    target_sources(max14906 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(max14906 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
    target_compile_definitions(max14906 PRIVATE BASIC_EXAMPLE)
endif()

# IIO example
if(CONFIG_MAX14906_IIO_EXAMPLE)
    target_sources(max14906 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(max14906 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
    target_compile_definitions(max14906 PRIVATE IIO_SUPPORT)
endif()

target_include_directories(max14906 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(max14906 no-os)

config_platform_sdk(max14906)
add_flash_target(max14906)
post_build_config(max14906)
