add_executable(ad74414h)

# Common sources
target_sources(ad74414h PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_AD74414H_BASIC_EXAMPLE)
    target_sources(ad74414h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad74414h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# Current input ext example
if(CONFIG_AD74414H_CURRENT_INPUT_EXT_EXAMPLE)
    target_sources(ad74414h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_input_ext/current_input_ext.c
    )
    target_include_directories(ad74414h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_input_ext
    )
endif()

# Current input loop example
if(CONFIG_AD74414H_CURRENT_INPUT_LOOP_EXAMPLE)
    target_sources(ad74414h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_input_loop/current_input_loop.c
    )
    target_include_directories(ad74414h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_input_loop
    )
endif()

# Current output example
if(CONFIG_AD74414H_CURRENT_OUTPUT_EXAMPLE)
    target_sources(ad74414h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_output/current_output.c
    )
    target_include_directories(ad74414h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/current_output
    )
endif()

# Digital input logic example
if(CONFIG_AD74414H_DIGITAL_INPUT_LOGIC_EXAMPLE)
    target_sources(ad74414h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_input_logic/digital_input_logic.c
    )
    target_include_directories(ad74414h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_input_logic
    )
endif()

# Digital input loop example
if(CONFIG_AD74414H_DIGITAL_INPUT_LOOP_EXAMPLE)
    target_sources(ad74414h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_input_loop/digital_input_loop.c
    )
    target_include_directories(ad74414h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_input_loop
    )
endif()

# Digital output example
if(CONFIG_AD74414H_DIGITAL_OUTPUT_EXAMPLE)
    target_sources(ad74414h PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_output/digital_output.c
    )
    target_include_directories(ad74414h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/digital_output
    )
endif()

target_include_directories(ad74414h PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad74414h no-os)

config_platform_sdk(ad74414h)
add_flash_target(ad74414h)
post_build_config(ad74414h)
