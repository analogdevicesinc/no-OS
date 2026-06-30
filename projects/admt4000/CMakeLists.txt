add_executable(admt4000)

# Common sources
target_sources(admt4000 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/iio_admt_evb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_ADMT4000_BASIC_EXAMPLE)
    target_sources(admt4000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(admt4000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO Trigger example
if(CONFIG_ADMT4000_IIO_TRIGGER_EXAMPLE)
    target_sources(admt4000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example/iio_trigger_example.c
        ${CMAKE_SOURCE_DIR}/libraries/precision-converters-library/common/common.c
        ${CMAKE_SOURCE_DIR}/libraries/precision-converters-library/board_info/board_info.c
    )
    target_include_directories(admt4000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example
        ${CMAKE_SOURCE_DIR}/libraries/precision-converters-library/common
        ${CMAKE_SOURCE_DIR}/libraries/precision-converters-library/board_info
        ${CMAKE_SOURCE_DIR}/libraries/precision-converters-library/sdp_k1_sdram
    )
    target_compile_definitions(admt4000 PRIVATE IIO_IGNORE_BUFF_OVERRUN_ERR)
    # precision-converters board_info.c uses sprintf without including
    # <stdio.h>; GCC 14+ treats implicit function declarations as errors.
    # Force-include stdio.h (SHELL: keeps "-include stdio.h" as two tokens).
    target_compile_options(admt4000 PRIVATE "$<$<COMPILE_LANGUAGE:C>:SHELL:-include stdio.h>")
endif()

target_include_directories(admt4000 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# TMC5240 support
if(CONFIG_ADMT4000_TMC5240)
    target_compile_definitions(admt4000 PRIVATE
        TMC=1
        TMC_DEVICE=TMC5240
    )
endif()

target_link_libraries(admt4000 no-os)

config_platform_sdk(admt4000)
add_flash_target(admt4000)
post_build_config(admt4000)
