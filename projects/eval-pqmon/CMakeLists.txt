add_executable(eval-pqmon)

target_sources(eval-pqmon PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/afe_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/afe_calibration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pqlib_example.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pqlib_afe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pqlib_convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/iio_pqm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/flash_storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
    # LCD driver used unconditionally by the common code (no_os_crc8 is already
    # part of the no-os util library).
    ${NO_OS_DIR}/drivers/display/nhd_c12832a1z/nhd_c12832a1z.c
)

if(CONFIG_EVAL_PQMON_BASIC_EXAMPLE)
    target_sources(eval-pqmon PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(eval-pqmon PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
    target_compile_definitions(eval-pqmon PRIVATE BASIC_EXAMPLE)
endif()

target_include_directories(eval-pqmon PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${NO_OS_DIR}/drivers/display/nhd_c12832a1z
    ${NO_OS_DIR}/network
)

# The IIO buffer-overrun error is expected during acquisition; suppress it.
target_compile_definitions(eval-pqmon PRIVATE IIO_IGNORE_BUFF_OVERRUN_ERR)

# Transport interface: each variant selects one PQM_CONN_* define. All
# networking sources are pulled in by the no-os tree from the CONFIG_* symbols
# in the variant's defconfig (CONFIG_W5500 / CONFIG_LWIP + CONFIG_ADIN1110,
# always with CONFIG_NO_OS_NET), so no per-project source wiring is needed.
if(CONFIG_EVAL_PQMON_CONN_USB)
    target_compile_definitions(eval-pqmon PRIVATE PQM_CONN_USB)
elseif(CONFIG_EVAL_PQMON_CONN_SERIAL)
    target_compile_definitions(eval-pqmon PRIVATE PQM_CONN_SERIAL)
elseif(CONFIG_EVAL_PQMON_CONN_ETH)
    # W5500 Ethernet (no_os_net HAL) keeps the USB console alive alongside the
    # wired link. The W5500 transport is plain TCP (no TLS).
    target_compile_definitions(eval-pqmon PRIVATE
        PQM_CONN_USB
        PQM_CONN_ETH
        DISABLE_SECURE_SOCKET
    )
elseif(CONFIG_EVAL_PQMON_CONN_T1L)
    # 10BASE-T1L over ADIN1110 (no_os_net HAL over lwIP). network/lwip and its
    # adin1110 netdev are compiled from CONFIG_LWIP + CONFIG_NO_OS_NET.
    target_compile_definitions(eval-pqmon PRIVATE PQM_CONN_T1L)
endif()

target_link_libraries(eval-pqmon no-os)

# Locate the prebuilt CMSIS-DSP archive matching the soft-float ABI. Search only
# under a DSP/ directory (recursively), matching the legacy Makefile's
#   find $(MAXIM_LIBRARIES)/CMSIS -path "*/DSP/*" -name libarm_cortexM4l_math.a
# The CMSIS/Lib/GCC/ copy is an older CMSIS-DSP that lacks symbols pqlib needs
# (e.g. arm_divide_q31), so it must NOT be used.
file(GLOB_RECURSE _cmsis_dsp_lib
    "$ENV{MAXIM_LIBRARIES}/CMSIS/*/DSP/*/libarm_cortexM4l_math.a")
if(_cmsis_dsp_lib)
    list(GET _cmsis_dsp_lib 0 _cmsis_dsp_lib)
else()
    message(FATAL_ERROR "CMSIS-DSP math library (libarm_cortexM4l_math.a under "
                        "a DSP/ directory) not found in MAXIM_LIBRARIES")
endif()

# The proprietary pqlib is not in the repo; its location is supplied via the
# PQLIB_PATH environment variable.
if(NOT DEFINED ENV{PQLIB_PATH} OR "$ENV{PQLIB_PATH}" STREQUAL "")
    message(FATAL_ERROR "PQLIB_PATH is not set. Point it at the directory that "
                        "contains libadi_pqlib_cm4_gcc.a and include/.")
endif()
set(_pqlib_path "$ENV{PQLIB_PATH}")

# pqlib references CMSIS-DSP routines (arm_rfft_q31, arm_cos_q15, ...), so the
# DSP archive must appear AFTER pqlib on the link line. Wrap both in a link
# group so the static-archive ordering is resolved regardless of sequence, and
# add libm for sqrtl.
target_include_directories(eval-pqmon PUBLIC
    ${_pqlib_path}/include
    ${_pqlib_path}/include/config
)
target_link_libraries(eval-pqmon
    -Wl,--start-group
    ${_pqlib_path}/libadi_pqlib_cm4_gcc.a
    ${_cmsis_dsp_lib}
    -Wl,--end-group
    m
)

config_platform_sdk(eval-pqmon)
add_flash_target(eval-pqmon)
post_build_config(eval-pqmon)
