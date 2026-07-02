add_executable(ltc2983)

# Common sources
target_sources(ltc2983 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_LTC2983_BASIC_EXAMPLE)
    target_sources(ltc2983 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ltc2983 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_LTC2983_IIO_EXAMPLE)
    target_sources(ltc2983 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(ltc2983 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

# ADT7604 Basic example
if(CONFIG_LTC2983_ADT7604_BASIC_EXAMPLE)
    target_sources(ltc2983 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/adt7604_basic/adt7604_basic_example.c
    )
    target_include_directories(ltc2983 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/adt7604_basic
    )
endif()

# ADT7604 IIO example
if(CONFIG_LTC2983_ADT7604_IIO_EXAMPLE)
    target_sources(ltc2983 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/adt7604_iio/adt7604_iio_example.c
    )
    target_include_directories(ltc2983 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/adt7604_iio
    )
endif()

target_include_directories(ltc2983 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ltc2983 no-os)

config_platform_sdk(ltc2983)
add_flash_target(ltc2983)
post_build_config(ltc2983)
