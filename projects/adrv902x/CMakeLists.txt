add_executable(adrv902x)

target_sources(adrv902x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/clkgen_routines.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal/no_os_platform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_ADRV902X_BASIC_EXAMPLE)
    target_sources(adrv902x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(adrv902x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

if(CONFIG_ADRV902X_DMA_EXAMPLE)
    target_sources(adrv902x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma_example/dma_example.c
    )
    target_include_directories(adrv902x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma_example
    )
endif()

if(CONFIG_ADRV902X_IIO_EXAMPLE)
    target_sources(adrv902x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(adrv902x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(adrv902x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
)

# Select the JESD use-case firmware profile from the bitstream itself, instead
# of hard-coding an include-path order. scripts/xsa_profile.sh reads LINK_MODE,
# ORx presence and the main-Rx converter count out of the .xsa hardware handoff
# and reports the matching firmware/ subdirectory. Only that one directory is
# put on the include path, so #include "ActiveUseCase_profile.h" resolves
# unambiguously and the Rx converter count matches the TPL core in the design.
execute_process(
    COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/scripts/xsa_profile.sh "${HARDWARE}"
    OUTPUT_VARIABLE ADRV902X_PROBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE ADRV902X_PROBE_RC
)
if(NOT ADRV902X_PROBE_RC EQUAL 0)
    message(FATAL_ERROR "adrv902x: failed to read JESD profile from '${HARDWARE}'")
endif()
string(REGEX MATCH "PROFILE=([A-Za-z0-9_]+)" _ "${ADRV902X_PROBE}")
set(ADRV902X_PROFILE "${CMAKE_MATCH_1}")
string(REGEX MATCH "RX_CONVS=([0-9]+)" _ "${ADRV902X_PROBE}")
set(ADRV902X_RX_CONVS "${CMAKE_MATCH_1}")
message(STATUS "adrv902x JESD profile (from XSA): ${ADRV902X_PROFILE}, main-Rx converters=${ADRV902X_RX_CONVS}")

if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/common/firmware/${ADRV902X_PROFILE})
    message(FATAL_ERROR
        "adrv902x: the bitstream needs the '${ADRV902X_PROFILE}' firmware profile, "
        "but src/common/firmware/${ADRV902X_PROFILE}/ does not exist. Generate its "
        "ActiveUseCase_profile.h from Madura TES (see README: Switching Between Use Cases).")
endif()

target_include_directories(adrv902x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/firmware/${ADRV902X_PROFILE}
)
target_compile_definitions(adrv902x PUBLIC
    ADRV9025_RX_JESD_CONVS_PER_DEVICE=${ADRV902X_RX_CONVS}
)
if(ADRV902X_PROFILE STREQUAL "JESD204C_ORx")
    target_compile_definitions(adrv902x PUBLIC JESD204C_PROFILE)
endif()

# The madura driver (in no-os library) includes the project-supplied no_os_platform.h
# which defines adrv9025_hal_cfg. It must be on the no-os include path.
target_include_directories(no-os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hal
)

config_platform_sdk(adrv902x)
add_flash_target(adrv902x)
post_build_config(adrv902x)
target_link_libraries(adrv902x no-os)
