add_executable(eval-adxl37x)

# Common sources
target_sources(eval-adxl37x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_EVAL_ADXL37X_BASIC_EXAMPLE)
    target_sources(eval-adxl37x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(eval-adxl37x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

# IIO example
if(CONFIG_EVAL_ADXL37X_IIO_EXAMPLE)
    target_sources(eval-adxl37x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(eval-adxl37x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(eval-adxl37x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-adxl37x no-os)

# The basic example keeps recorded_data[MAX_SAMPLES] on the stack (3072 bytes at
# AXES_USED 1) and stdout is unbuffered, so newlib routes printf through
# __sbprintf, which puts a further BUFSIZ scratch buffer on the stack. Together
# with _vfprintf_r that overruns the 4 kB default from startup_max${TARGET_NUM}.S
# and corrupts the string being printed. __STACK_SIZE is read by the
# preprocessor in the startup assembly, which is a source of the no-os library,
# so the define has to go on that target rather than on this executable.
if(PLATFORM STREQUAL "maxim")
    set(EVAL_ADXL37X_STACK_SIZE 0x2000 CACHE STRING
        "Stack size in bytes for the eval-adxl37x firmware")
    target_compile_definitions(no-os PRIVATE __STACK_SIZE=${EVAL_ADXL37X_STACK_SIZE})
endif()

config_platform_sdk(eval-adxl37x)
add_flash_target(eval-adxl37x)
post_build_config(eval-adxl37x)
