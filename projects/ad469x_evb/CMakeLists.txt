add_executable(ad469x_evb)

target_sources(ad469x_evb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_AD469X_EVB_BASIC_EXAMPLE)
    target_sources(ad469x_evb PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad469x_evb PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

if(CONFIG_AD469X_EVB_IIO_EXAMPLE)
    target_sources(ad469x_evb PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(ad469x_evb PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
    target_compile_definitions(ad469x_evb PRIVATE IIO_EXAMPLE)
endif()

if(CONFIG_AD469X_EVB_STANDARD_SEQ_EXAMPLE)
    target_sources(ad469x_evb PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad469x_evb PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
    target_compile_definitions(ad469x_evb PRIVATE AD469X_SEQ_STANDARD)
endif()

if(CONFIG_AD469X_EVB_ADVANCE_SEQ_EXAMPLE)
    target_sources(ad469x_evb PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad469x_evb PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
    target_compile_definitions(ad469x_evb PRIVATE AD469X_SEQ_ADVANCED)
endif()

target_include_directories(ad469x_evb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad469x_evb no-os)

config_platform_sdk(ad469x_evb)
add_flash_target(ad469x_evb)
post_build_config(ad469x_evb)
