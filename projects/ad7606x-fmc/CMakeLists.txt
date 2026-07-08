add_executable(ad7606x-fmc)

target_sources(ad7606x-fmc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common_data.c
)

if(CONFIG_AD7606X_FMC_BASIC_EXAMPLE)
    target_sources(ad7606x-fmc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/basic_example.c
    )
endif()

if(CONFIG_AD7606X_FMC_IIO_EXAMPLE)
    target_sources(ad7606x-fmc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/iio_example.c
    )
endif()

target_include_directories(ad7606x-fmc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad7606x-fmc no-os)

config_platform_sdk(ad7606x-fmc)
add_flash_target(ad7606x-fmc)
post_build_config(ad7606x-fmc)
