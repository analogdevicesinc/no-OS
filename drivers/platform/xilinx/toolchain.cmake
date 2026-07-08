# Xilinx (Vitis) bare-metal toolchain file.
#
# Unlike maxim/stm32, the target CPU architecture is not known up front -- it is
# encoded in the .xsa hardware-description file. We determine it here by running
# the HSI-based get_arch helper (tools/scripts/platform/xilinx/util.tcl) via
# xsct, then pick the matching bare-metal compiler shipped inside the Vitis
# install. The BSP + linker script are generated later, at configure time, by
# config_xilinx_sdk() (cmake/xilinx/xilinx_platform_sdk.cmake).

if(NOT DEFINED ENV{XILINX_VITIS})
    message(FATAL_ERROR
        "XILINX_VITIS is not set. Source settings64.sh from your Vitis install "
        "(e.g. source ~/.xilinx/<ver>/Vitis/settings64.sh) before configuring.")
endif()
set(XILINX_VITIS "$ENV{XILINX_VITIS}")

# HARDWARE (absolute path to the .xsa) is required: the arch and BSP both come
# from it. It can be supplied three ways (in priority order):
#   -DHARDWARE=/abs/path/system_top.xsa   (explicit)
#   HARDWARE=/abs/path env var            (CI / machine-specific, like MAXIM_LIBRARIES)
# The .xsa is a per-board, machine-local artifact (downloaded from the HDL
# server on CI), so it is NOT hardcoded in the repo/preset.
if((NOT DEFINED HARDWARE OR "${HARDWARE}" STREQUAL "") AND DEFINED ENV{HARDWARE})
    set(HARDWARE "$ENV{HARDWARE}")
endif()
#
# CMake re-includes this toolchain file for its internal try_compile ABI probe,
# a sub-project that does NOT inherit top-level cache/-D variables unless they
# are listed in CMAKE_TRY_COMPILE_PLATFORM_VARIABLES. Cache HARDWARE (and the
# derived XILINX_ARCH) and register them so the probe sees the same values.
set(HARDWARE "${HARDWARE}" CACHE FILEPATH "Path to the board .xsa")
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES HARDWARE XILINX_ARCH)

if(NOT DEFINED HARDWARE OR "${HARDWARE}" STREQUAL "")
    message(FATAL_ERROR
        "HARDWARE is not set. Point it at the board .xsa file "
        "(-DHARDWARE=/abs/path/system_top.xsa or set it in the board preset).")
endif()
if(NOT EXISTS "${HARDWARE}")
    message(FATAL_ERROR "HARDWARE .xsa does not exist: ${HARDWARE}")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
# Bare-metal: only compile (not link) during CMake's compiler test -- there is
# no linker script or BSP yet at this point.
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# --- Determine the processor architecture from the XSA (once, cached) ---------
# get_arch writes arch.txt (e.g. "ps7_cortexa9_0") next to the XSA copy.
if(NOT DEFINED XILINX_ARCH)
    get_filename_component(_xsa_dir "${HARDWARE}" DIRECTORY)
    get_filename_component(_xsa_file "${HARDWARE}" NAME)
    set(_arch_work "${CMAKE_BINARY_DIR}/xsa_arch")
    file(MAKE_DIRECTORY "${_arch_work}")
    file(COPY "${HARDWARE}" DESTINATION "${_arch_work}")

    find_program(XSCT_EXECUTABLE xsct HINTS "${XILINX_VITIS}/bin")
    if(NOT XSCT_EXECUTABLE)
        message(FATAL_ERROR "xsct not found under ${XILINX_VITIS}/bin")
    endif()

    execute_process(
        COMMAND ${XSCT_EXECUTABLE} -nodisp
                "${CMAKE_CURRENT_LIST_DIR}/../../../tools/scripts/platform/xilinx/util.tcl"
                get_arch "${_arch_work}" "${_arch_work}" "${_xsa_file}"
        RESULT_VARIABLE _arch_rc
        OUTPUT_QUIET)
    if(NOT _arch_rc EQUAL 0)
        message(FATAL_ERROR "xsct get_arch failed on ${HARDWARE} (rc=${_arch_rc})")
    endif()
    file(READ "${_arch_work}/arch.txt" _arch_raw)
    string(STRIP "${_arch_raw}" XILINX_ARCH)
    set(XILINX_ARCH "${XILINX_ARCH}" CACHE INTERNAL "Xilinx processor arch from XSA")
endif()
message(STATUS "Xilinx arch (from XSA): ${XILINX_ARCH}")

# --- Select compiler + CPU flags per arch -------------------------------------
if(XILINX_ARCH MATCHES "cortexa9")
    set(_xl_prefix arm-none-eabi)
    file(GLOB _xl_bin "${XILINX_VITIS}/gnu/aarch32/lin/gcc-arm-none-eabi/bin")
    set(_xl_cpu_flags "-mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard")
elseif(XILINX_ARCH MATCHES "cortexa53")
    set(_xl_prefix aarch64-none-elf)
    file(GLOB _xl_bin "${XILINX_VITIS}/gnu/aarch64/lin/aarch64-none/bin")
    set(_xl_cpu_flags "")
elseif(XILINX_ARCH MATCHES "cortexr5")
    set(_xl_prefix armr5-none-eabi)
    file(GLOB _xl_bin "${XILINX_VITIS}/gnu/armr5/lin/gcc-arm-none-eabi/bin")
    set(_xl_cpu_flags "-mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16")
elseif(XILINX_ARCH MATCHES "sys_mb")
    set(_xl_prefix microblaze-xilinx-elf)
    file(GLOB _xl_bin "${XILINX_VITIS}/gnu/microblaze/lin/bin")
    set(_xl_cpu_flags "-mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mno-xl-soft-div -mcpu=v11.0 -mno-xl-soft-mul -mxl-multiply-high")
else()
    message(FATAL_ERROR "Unsupported Xilinx arch '${XILINX_ARCH}'")
endif()

find_program(CMAKE_C_COMPILER   ${_xl_prefix}-gcc     HINTS ${_xl_bin})
find_program(CMAKE_CXX_COMPILER ${_xl_prefix}-g++     HINTS ${_xl_bin})
find_program(CMAKE_ASM_COMPILER ${_xl_prefix}-gcc     HINTS ${_xl_bin})
find_program(CMAKE_OBJCOPY      ${_xl_prefix}-objcopy HINTS ${_xl_bin})
find_program(CMAKE_SIZE         ${_xl_prefix}-size    HINTS ${_xl_bin})

if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR
        "Xilinx bare-metal compiler ${_xl_prefix}-gcc not found under ${_xl_bin}")
endif()

set(CMAKE_C_FLAGS_INIT   "${_xl_cpu_flags}")
set(CMAKE_CXX_FLAGS_INIT "${_xl_cpu_flags}")
set(CMAKE_ASM_FLAGS_INIT "${_xl_cpu_flags}")

# Emit the executable as <project>.elf, matching the maxim/stm32/pico
# toolchains. The CI success gate (build_projects.py) and FlashTools
# (${TARGET_NAME}.elf) both key off the .elf name; .hex/.bin are derived from
# $<TARGET_FILE:...> so they are unaffected by the suffix.
set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C   ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")
