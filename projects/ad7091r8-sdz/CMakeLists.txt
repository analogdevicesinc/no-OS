add_executable(ad7091r8-sdz)

# Common sources
target_sources(ad7091r8-sdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_AD7091R8_SDZ_BASIC_EXAMPLE)
    target_sources(ad7091r8-sdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad7091r8-sdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_AD7091R8_SDZ_IIO_EXAMPLE)
    target_sources(ad7091r8-sdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(ad7091r8-sdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

# IIO timer trigger example
if(CONFIG_AD7091R8_SDZ_IIO_TIMER_TRIGGER_EXAMPLE)
    target_sources(ad7091r8-sdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_timer_trigger/iio_timer_trigger_example.c
    )
    target_include_directories(ad7091r8-sdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_timer_trigger
    )
    target_compile_definitions(ad7091r8-sdz PRIVATE IIO_TIMER_TRIGGER_EXAMPLE)
endif()

# Device variant compile definitions
if(CONFIG_AD7091R8_SDZ_DEV_R2)
    target_compile_definitions(ad7091r8-sdz PRIVATE AD7091R2_DEV)
elseif(CONFIG_AD7091R8_SDZ_DEV_R4)
    target_compile_definitions(ad7091r8-sdz PRIVATE AD7091R4_DEV)
else()
    target_compile_definitions(ad7091r8-sdz PRIVATE AD7091R8_DEV)
endif()

target_include_directories(ad7091r8-sdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad7091r8-sdz no-os)

config_platform_sdk(ad7091r8-sdz)
add_flash_target(ad7091r8-sdz)
post_build_config(ad7091r8-sdz)
