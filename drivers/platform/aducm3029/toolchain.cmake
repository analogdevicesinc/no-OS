# ADuCM3029 (aducm) platform toolchain
#
# The ADuCM3029 build relies on Analog Devices CrossCore Embedded Studio (CCES)
# both for the cross compiler / OpenOCD and (later, in the platform SDK module)
# for generating the project startup, pinmux, RTE config and linker script.
# This file mirrors the discovery the legacy tools/scripts/aducm.mk performs.

message(STATUS "ADuCM3029 Platform")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# For bare-metal toolchains, only compile (not link) during CMake's compiler
# test. The link step needs the CCES-generated startup/linker script which does
# not exist yet at toolchain-detection time.
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# ---------------------------------------------------------------------------
# Locate CrossCore Embedded Studio (CCES_HOME)
# ---------------------------------------------------------------------------
if(NOT CCES_HOME)
    if(DEFINED ENV{CCES_HOME})
        set(CCES_HOME "$ENV{CCES_HOME}")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        file(GLOB _cces_candidates "C:/Analog Devices/CrossCore Embedded Studio*")
    else()
        file(GLOB _cces_candidates "/opt/analog/cces/*")
    endif()

    if(NOT CCES_HOME AND _cces_candidates)
        # Pick the highest (last) version directory.
        list(SORT _cces_candidates)
        list(GET _cces_candidates -1 CCES_HOME)
    endif()
endif()

if(NOT CCES_HOME OR NOT EXISTS "${CCES_HOME}")
    message(FATAL_ERROR
        "CCES_HOME not found. Install Analog Devices CrossCore Embedded Studio "
        "and set the CCES_HOME environment variable (e.g. "
        "export CCES_HOME=/opt/analog/cces/3.0.3) or pass -DCCES_HOME=...")
endif()

set(CCES_HOME "${CCES_HOME}" CACHE PATH "CrossCore Embedded Studio install root" FORCE)
message(STATUS "CCES_HOME: ${CCES_HOME}")

# Tool / SDK locations derived from CCES_HOME (mirrors aducm.mk:27-50).
set(CCES_COMPILER_BIN "${CCES_HOME}/ARM/gcc-arm-embedded/bin")
set(CCES_OPENOCD_BIN "${CCES_HOME}/ARM/openocd/bin")
set(CCES_OPENOCD_SCRIPTS "${CCES_HOME}/ARM/openocd/share/openocd/scripts")
set(CCES_EXE "${CCES_HOME}/Eclipse" CACHE PATH "CCES Eclipse directory" FORCE)

# Device Family Pack and CMSIS Core (globbed: versioned directories).
file(GLOB _aducm_dfp "${CCES_HOME}/ARM/packs/AnalogDevices/ADuCM302x_DFP/*")
if(_aducm_dfp)
    list(SORT _aducm_dfp)
    list(GET _aducm_dfp -1 ADUCM_DFP)
endif()
file(GLOB _cmsis_core "${CCES_HOME}/ARM/packs/ARM/CMSIS/*/CMSIS/Core")
if(_cmsis_core)
    list(SORT _cmsis_core)
    list(GET _cmsis_core -1 ADUCM_CMSIS_CORE)
endif()

if(NOT ADUCM_DFP OR NOT EXISTS "${ADUCM_DFP}")
    message(FATAL_ERROR
        "ADuCM302x Device Family Pack not found under "
        "${CCES_HOME}/ARM/packs/AnalogDevices/ADuCM302x_DFP. "
        "Install it via the CCES pack manager.")
endif()
if(NOT ADUCM_CMSIS_CORE OR NOT EXISTS "${ADUCM_CMSIS_CORE}")
    message(FATAL_ERROR
        "CMSIS Core pack not found under ${CCES_HOME}/ARM/packs/ARM/CMSIS. "
        "Install it via the CCES pack manager.")
endif()

# Export for the platform SDK module and CMakeLists (CCES headless gen, includes).
set(ADUCM_DFP "${ADUCM_DFP}" CACHE PATH "ADuCM302x Device Family Pack" FORCE)
set(ADUCM_CMSIS_CORE "${ADUCM_CMSIS_CORE}" CACHE PATH "CMSIS Core pack" FORCE)
message(STATUS "ADuCM DFP: ${ADUCM_DFP}")
message(STATUS "CMSIS Core: ${ADUCM_CMSIS_CORE}")

# ---------------------------------------------------------------------------
# Compiler selection
# ---------------------------------------------------------------------------
if(NOT DEFINED USE_VENDOR_TOOLCHAIN)
    set(USE_VENDOR_TOOLCHAIN ON)
endif()

if(USE_VENDOR_TOOLCHAIN)
    set(_cc_hints "${CCES_COMPILER_BIN}")
else()
    set(_cc_hints "")
endif()

find_program(CMAKE_C_COMPILER   arm-none-eabi-gcc     HINTS ${_cc_hints})
find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++     HINTS ${_cc_hints})
find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc     HINTS ${_cc_hints})
find_program(CMAKE_LINKER       arm-none-eabi-ld      HINTS ${_cc_hints})
find_program(CMAKE_SIZE         arm-none-eabi-size    HINTS ${_cc_hints})
find_program(CMAKE_OBJCOPY      arm-none-eabi-objcopy HINTS ${_cc_hints})
find_program(CMAKE_AR           arm-none-eabi-ar      HINTS ${_cc_hints})

if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR
        "arm-none-eabi-gcc not found (searched ${CCES_COMPILER_BIN} and PATH). "
        "Check the CCES install, or configure with -DUSE_VENDOR_TOOLCHAIN=OFF to "
        "use a compiler already on PATH.")
endif()

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# ---------------------------------------------------------------------------
# Compiler / linker flags (from aducm.mk:97-117)
# ---------------------------------------------------------------------------
set(COMMON_CPU_FLAGS "-mcpu=cortex-m3 -mthumb")

# Definitions required by the ADuCM3029 RTE / DFP headers.
set(ADUCM_DEFS "-DCORE0 -D_RTE_ -D__ADUCM3029__ -D__SILICON_REVISION__=0xffff")

set(CMAKE_C_FLAGS "${COMMON_CPU_FLAGS} ${ADUCM_DEFS} -ffunction-sections -fdata-sections -MD" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "${COMMON_CPU_FLAGS} ${ADUCM_DEFS} -ffunction-sections -fdata-sections -MD" CACHE STRING "C++ compiler flags" FORCE)
set(CMAKE_ASM_FLAGS "${COMMON_CPU_FLAGS} ${ADUCM_DEFS} -x assembler-with-cpp" CACHE STRING "ASM compiler flags" FORCE)

# Debug build flags - Full debug info, no optimization
set(CMAKE_C_FLAGS_DEBUG "-g -gdwarf-2 -O0 -D_DEBUG" CACHE STRING "C compiler flags for Debug" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g -gdwarf-2 -O0 -D_DEBUG" CACHE STRING "C++ compiler flags for Debug" FORCE)
set(CMAKE_ASM_FLAGS_DEBUG "-g -gdwarf-2 -D_DEBUG" CACHE STRING "ASM compiler flags for Debug" FORCE)

# Release build flags - Optimize for speed, disable assertions
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C compiler flags for Release" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C++ compiler flags for Release" FORCE)
set(CMAKE_ASM_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "ASM compiler flags for Release" FORCE)

# RelWithDebInfo build flags - Optimize with debug info
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -gdwarf-2 -DNDEBUG" CACHE STRING "C compiler flags for RelWithDebInfo" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -gdwarf-2 -DNDEBUG" CACHE STRING "C++ compiler flags for RelWithDebInfo" FORCE)
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO "-g -gdwarf-2 -DNDEBUG" CACHE STRING "ASM compiler flags for RelWithDebInfo" FORCE)

# MinSizeRel build flags - Optimize for size
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C compiler flags for MinSizeRel" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C++ compiler flags for MinSizeRel" FORCE)
set(CMAKE_ASM_FLAGS_MINSIZEREL "-DNDEBUG" CACHE STRING "ASM compiler flags for MinSizeRel" FORCE)

# Linker flags. The -T<linker script> is intentionally NOT set here: the
# ADuCM3029.ld is generated per-project by config_aducm3029_sdk(), which appends
# it via target_link_options() (mirrors the stm32/CubeMX flow).
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_CPU_FLAGS} -specs=nosys.specs -Wl,--gc-sections -lm" CACHE STRING "Linker flags for MCU" FORCE)

# ---------------------------------------------------------------------------
# OpenOCD discovery (for flash/debug targets)
# ---------------------------------------------------------------------------
find_program(OPENOCD_PATH
    NAMES openocd
    HINTS "${CCES_OPENOCD_BIN}"
    DOC "Path to OpenOCD executable"
)

if(OPENOCD_PATH)
    set(OPENOCD_SCRIPTS "${CCES_OPENOCD_SCRIPTS}")
    set(OPENOCD_INTERFACE "interface/cmsis-dap.cfg")
    set(OPENOCD_CHIPNAME "aducm3029")
    set(OPENOCD_TARGETCFG "target/aducm3029.cfg")

    # target/aducm3029.cfg ships in the DFP's own openocd scripts directory, not
    # the standard CCES scripts dir (-s OPENOCD_SCRIPTS). Add it to OpenOCD's
    # search path so `source [find target/aducm3029.cfg]` resolves (mirrors the
    # second -s in tools/scripts/aducm.mk).
    set(OPENOCD_EXTRA_COMMANDS "add_script_search_dir \"${ADUCM_DFP}/openocd/scripts\"")

    if(NOT PROBE)
        set(PROBE "openocd")
    endif()

    message(STATUS "Found OpenOCD: ${OPENOCD_PATH}")
else()
    message(STATUS "OpenOCD not found in ${CCES_OPENOCD_BIN}")
endif()
