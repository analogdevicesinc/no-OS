# CFS path can be set via -DCFS_PATH=..., the CFS_PATH environment variable, or auto-detected
find_path(CFS
          NAMES
            cfs.json
          PATHS
            "${CFS_PATH}" "$ENV{CFS_PATH}" "$ENV{HOME}/cfs"
)

if(DEFINED MAXIM_LIBRARIES)
    # Explicit SDK paths also work in nested coprocessor builds.
elseif (CFS)
    set(MAXIM_LIBRARIES ${CFS}/SDK/MAX/Libraries)
elseif(DEFINED ENV{MAXIM_LIBRARIES})
    set(MAXIM_LIBRARIES $ENV{MAXIM_LIBRARIES})
else()
    message(FATAL_ERROR
        "MAXIM_LIBRARIES is not set. Point it at the Maxim SDK Libraries directory "
        "(set the MAXIM_LIBRARIES environment variable), or provide a CFS install "
        "via -DCFS_PATH=... / the CFS_PATH environment variable.")
endif()

# MAXIM_LIBRARIES is dereferenced into the linker script path (-T...) below; a
# wrong path would otherwise fail much later at link time with a missing .ld.
if(NOT EXISTS "${MAXIM_LIBRARIES}")
    message(FATAL_ERROR "MAXIM_LIBRARIES directory does not exist: ${MAXIM_LIBRARIES}")
endif()

message(STATUS "MAXIM_LIBRARIES: ${MAXIM_LIBRARIES}")

set(CMAKE_SYSTEM_NAME Generic)
set(MAXIM_CORE "arm" CACHE STRING "Maxim CPU core (arm or riscv)")
set_property(CACHE MAXIM_CORE PROPERTY STRINGS arm riscv)
if(DEFINED MAXIM_CONFIGURED_CORE AND NOT MAXIM_CONFIGURED_CORE STREQUAL MAXIM_CORE)
    message(FATAL_ERROR "Changing MAXIM_CORE requires a separate build directory")
endif()
set(MAXIM_CONFIGURED_CORE ${MAXIM_CORE} CACHE INTERNAL "Configured Maxim core")
if(NOT MAXIM_CORE MATCHES "^(arm|riscv)$")
    message(FATAL_ERROR "Unsupported MAXIM_CORE=${MAXIM_CORE}; select arm or riscv")
endif()
if(MAXIM_CORE STREQUAL "riscv" AND NOT TARGET_NUM STREQUAL "78000")
    message(FATAL_ERROR "RISC-V platform support is currently available for MAX78000 only")
endif()
set(CMAKE_SYSTEM_PROCESSOR ${MAXIM_CORE})
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
    MAXIM_CORE MAXIM_LIBRARIES TARGET TARGET_NUM USE_VENDOR_TOOLCHAIN
    MAXIM_RISCV_FLASH_ORIGIN MAXIM_RISCV_FLASH_LENGTH MAXIM_RISCV_TIME_TIMER
    MAXIM_RISCV_SRAM2_RESERVE)

# For bare-metal toolchains, only compile (not link) during CMake's compiler test.
# This avoids linker failures caused by missing startup files, linker scripts,
# and syscall stubs (e.g. _sbrk needing 'end') during the detection phase.
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/max${TARGET_NUM}/memory_layout.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/max${TARGET_NUM}/memory_layout.cmake")
endif()

if(MAXIM_CORE STREQUAL "riscv")
    set(_maxim_prefix riscv-none-elf)
    if(CFS)
        set(CROSS_COMPILER_BIN "${CFS}/Tools/gcc/riscv-none-elf/bin")
    else()
        file(GLOB CROSS_COMPILER_BIN
            "${MAXIM_LIBRARIES}/../Tools/xPack/riscv-none-elf-gcc/*/bin"
            "${MAXIM_LIBRARIES}/../Tools/xPack/riscv-none-elf-gcc/bin")
    endif()
else()
    set(_maxim_prefix arm-none-eabi)
    if(CFS)
        set(CROSS_COMPILER_BIN "${CFS}/Tools/gcc/arm-none-eabi/bin")
    else()
        file(GLOB CROSS_COMPILER_BIN
            "${MAXIM_LIBRARIES}/../Tools/GNUTools/*/bin"
            "${MAXIM_LIBRARIES}/../Tools/GNUTools/bin")
    endif()
endif()

if(NOT USE_VENDOR_TOOLCHAIN)
    set(CROSS_COMPILER_BIN "")
endif()
find_program(CMAKE_C_COMPILER ${_maxim_prefix}-gcc HINTS ${CROSS_COMPILER_BIN})
find_program(CMAKE_CXX_COMPILER ${_maxim_prefix}-g++ HINTS ${CROSS_COMPILER_BIN})
find_program(CMAKE_ASM_COMPILER ${_maxim_prefix}-gcc HINTS ${CROSS_COMPILER_BIN})
find_program(CMAKE_LINKER ${_maxim_prefix}-ld HINTS ${CROSS_COMPILER_BIN})
find_program(CMAKE_SIZE ${_maxim_prefix}-size HINTS ${CROSS_COMPILER_BIN})
find_program(CMAKE_OBJCOPY ${_maxim_prefix}-objcopy HINTS ${CROSS_COMPILER_BIN})
find_program(CMAKE_NM ${_maxim_prefix}-nm HINTS ${CROSS_COMPILER_BIN})

if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR
        "${_maxim_prefix}-gcc not found under CROSS_COMPILER_BIN=${CROSS_COMPILER_BIN} "
        "or on PATH. Install the ${MAXIM_CORE} cross toolchain or check the SDK path.")
endif()

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# CPU flags depending on target. Nearly all Maxim parts are Cortex-M4. MAX32657 is Cortex-M33.
if(MAXIM_CORE STREQUAL "riscv")
    set(COMMON_CPU_FLAGS "-march=rv32imc_zicsr_zifencei -mabi=ilp32")
elseif(TARGET_NUM STREQUAL "32657")
    set(COMMON_CPU_FLAGS "-mthumb -mcpu=cortex-m33 -mfloat-abi=softfp -mfpu=fpv5-sp-d16 -mno-unaligned-access")
else()
    set(COMMON_CPU_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16")
endif()

# Common flags for all build types
set(CMAKE_C_FLAGS "${COMMON_CPU_FLAGS} -ffunction-sections -fdata-sections -MD" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "${COMMON_CPU_FLAGS} -ffunction-sections -fdata-sections -MD" CACHE STRING "C++ compiler flags" FORCE)
set(CMAKE_ASM_FLAGS "${COMMON_CPU_FLAGS} -x assembler-with-cpp" CACHE STRING "ASM compiler flags" FORCE)

# Debug build flags - Full debug info, no optimization
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -DDEBUG" CACHE STRING "C compiler flags for Debug" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG" CACHE STRING "C++ compiler flags for Debug" FORCE)
set(CMAKE_ASM_FLAGS_DEBUG "-g3" CACHE STRING "ASM compiler flags for Debug" FORCE)

# Release build flags - Optimize for speed, disable assertions
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C compiler flags for Release" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C++ compiler flags for Release" FORCE)
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE STRING "ASM compiler flags for Release" FORCE)

# RelWithDebInfo build flags - Optimize with debug info
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "C compiler flags for RelWithDebInfo" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "C++ compiler flags for RelWithDebInfo" FORCE)
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO "-g" CACHE STRING "ASM compiler flags for RelWithDebInfo" FORCE)

# MinSizeRel build flags - Optimize for size
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C compiler flags for MinSizeRel" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C++ compiler flags for MinSizeRel" FORCE)
set(CMAKE_ASM_FLAGS_MINSIZEREL "" CACHE STRING "ASM compiler flags for MinSizeRel" FORCE)

# Linker flags and memory layout are selected before compiler detection.
set(_maxim_gcc "${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC")
if(MAXIM_CORE STREQUAL "riscv")
    set(MAXIM_RISCV_FLASH_ORIGIN "0x10000000" CACHE STRING "CPU1 flash link address")
    set(MAXIM_RISCV_FLASH_LENGTH "0x80000" CACHE STRING "CPU1 flash region length")
    set(MAXIM_RISCV_TIME_TIMER "3" CACHE STRING "CPU1 elapsed-time timer (0..3)")
    set(MAXIM_RISCV_SRAM2_RESERVE "0" CACHE STRING "Reserved bytes at the start of CPU1 SRAM2")
    if(NOT MAXIM_RISCV_TIME_TIMER MATCHES "^[0-3]$")
        message(FATAL_ERROR "MAXIM_RISCV_TIME_TIMER must be between 0 and 3")
    endif()
    math(EXPR _rv_flash_end "${MAXIM_RISCV_FLASH_ORIGIN} + ${MAXIM_RISCV_FLASH_LENGTH}")
    math(EXPR _rv_flash_size "${MAXIM_RISCV_FLASH_LENGTH}")
    math(EXPR _rv_flash_start "${MAXIM_RISCV_FLASH_ORIGIN}")
    if(_rv_flash_start LESS 0x10000000 OR _rv_flash_size LESS_EQUAL 0 OR
       _rv_flash_end GREATER 0x10080000)
        message(FATAL_ERROR "CPU1 flash region is outside MAX78000 flash")
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/buildrv")
    file(WRITE "${CMAKE_BINARY_DIR}/buildrv/common_riscv.ld"
        "__FlashStart = ${MAXIM_RISCV_FLASH_ORIGIN};\n__FlashLength = ${MAXIM_RISCV_FLASH_LENGTH};\n")
    math(EXPR _rv_sram_reserve "${MAXIM_RISCV_SRAM2_RESERVE}")
    math(EXPR _rv_sram_alignment "${_rv_sram_reserve} % 16")
    if(_rv_sram_reserve LESS 0 OR _rv_sram_reserve GREATER_EQUAL 0xC000 OR _rv_sram_alignment)
        message(FATAL_ERROR "CPU1 SRAM2 reservation must be a multiple of 16 in [0, 0xC000)")
    endif()
    math(EXPR _rv_sram_origin "0x20010000 + ${_rv_sram_reserve}" OUTPUT_FORMAT HEXADECIMAL)
    math(EXPR _rv_sram_length "0xC000 - ${_rv_sram_reserve}" OUTPUT_FORMAT HEXADECIMAL)
    file(READ "${_maxim_gcc}/common.ld" _rv_memory)
    string(REGEX REPLACE
        "ORIGIN[ \t]*=[ \t]*0x20010000,[ \t]*LENGTH[ \t]*=[ \t]*0x0000[Cc]000"
        "ORIGIN = ${_rv_sram_origin}, LENGTH = ${_rv_sram_length}"
        _rv_memory "${_rv_memory}")
    if(_rv_sram_reserve GREATER 0 AND NOT _rv_memory MATCHES "ORIGIN = ${_rv_sram_origin}, LENGTH = ${_rv_sram_length}")
        message(FATAL_ERROR "SDK common.ld has an unsupported SRAM2 layout")
    endif()
    file(WRITE "${CMAKE_BINARY_DIR}/common.ld" "${_rv_memory}")
    set(_maxim_link_flags "-nostartfiles -L${CMAKE_BINARY_DIR} -L${_maxim_gcc} -T${_maxim_gcc}/${TARGET}_riscv.ld")
else()
    set(_maxim_link_flags "-specs=nosys.specs -T${_maxim_gcc}/${TARGET}.ld")
endif()
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_CPU_FLAGS} ${_maxim_link_flags} -Wl,--gc-sections,--undefined=_sbrk ${MCU_LINKER_FLAGS} --entry=Reset_Handler"
    CACHE STRING "Linker flags for MCU" FORCE)

# Work around a Ninja restat bug with arm-none-eabi-gcc: on link failure the
# toolchain deletes the output .elf, and Ninja restat then reports success
# because the output is "unchanged".  Wrapping the link command ensures a
# failure always leaves a touched output so restat never masks the error.
if(CMAKE_GENERATOR MATCHES "Ninja")
    set(CMAKE_C_LINK_EXECUTABLE
        "( <CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ) || ( <CMAKE_COMMAND> -E touch <TARGET> && false )"
    )
    set(CMAKE_CXX_LINK_EXECUTABLE
        "( <CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ) || ( <CMAKE_COMMAND> -E touch <TARGET> && false )"
    )
endif()

# Find OpenOCD - handles different names on Linux (openocd) and Windows (openocd.exe)
if (CFS)
    cmake_path(SET OPENOCD_SEARCH_PATH NORMALIZE "${CFS}/Tools/openocd")
else()
    cmake_path(SET OPENOCD_SEARCH_PATH NORMALIZE "${MAXIM_LIBRARIES}/../Tools/OpenOCD")
endif()

find_program(OPENOCD_PATH
    NAMES openocd
    HINTS ${OPENOCD_SEARCH_PATH}
    PATH_SUFFIXES bin
    DOC "Path to OpenOCD executable"
)

if(OPENOCD_PATH)
    if (CFS)
        cmake_path(SET OPENOCD_SCRIPTS NORMALIZE "${CFS}/Tools/openocd/share/openocd/scripts")
    else()
        cmake_path(SET OPENOCD_SCRIPTS NORMALIZE "${MAXIM_LIBRARIES}/../Tools/OpenOCD/scripts")
    endif()

    if (NOT DEFINED OPENOCD_INTERFACE)
        set(OPENOCD_INTERFACE "interface/cmsis-dap.cfg")
    endif()
    set(OPENOCD_CHIPNAME ${TARGET})
    set(OPENOCD_TARGETCFG "target/${TARGET}.cfg")

    if (NOT PROBE)
        set(PROBE "openocd")
    endif()

    message(STATUS "Found OpenOCD: ${OPENOCD_PATH}")
else()
    message(STATUS "OpenOCD not found in ${OPENOCD_SEARCH_PATH}")
endif()
