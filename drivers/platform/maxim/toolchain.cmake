# CFS path can be set via -DCFS_PATH=..., the CFS_PATH environment variable, or auto-detected
find_path(CFS
          NAMES
            cfs.json
          PATHS
            "${CFS_PATH}" "$ENV{CFS_PATH}" "$ENV{HOME}/cfs"
)

if (CFS)
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
set(CMAKE_SYSTEM_PROCESSOR arm)

# For bare-metal toolchains, only compile (not link) during CMake's compiler test.
# This avoids linker failures caused by missing startup files, linker scripts,
# and syscall stubs (e.g. _sbrk needing 'end') during the detection phase.
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/max${TARGET_NUM}/memory_layout.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/max${TARGET_NUM}/memory_layout.cmake")
endif()

if(USE_VENDOR_TOOLCHAIN)
    if(CFS)
        cmake_path(SET CROSS_COMPILER_BIN NORMALIZE "${CFS}/Tools/gcc/arm-none-eabi/bin")
    else()
        # MaximSDK installs the bundled GCC under a versioned directory
        # (Tools/GNUTools/<version>/bin), so the path must be globbed --
        # cmake_path(SET) would keep the literal '*'.
        file(GLOB CROSS_COMPILER_BIN "${MAXIM_LIBRARIES}/../Tools/GNUTools/*/bin")
    endif()

    find_program(CMAKE_C_COMPILER arm-none-eabi-gcc HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++ HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_LINKER arm-none-eabi-ld HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_SIZE arm-none-eabi-size HINTS ${CROSS_COMPILER_BIN})
    find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy HINTS ${CROSS_COMPILER_BIN})

    # Report a missing cross-compiler here, with the directory we searched,
    # rather than letting CMake fail later with a generic compiler-detection error.
    if(NOT CMAKE_C_COMPILER)
        message(FATAL_ERROR
            "arm-none-eabi-gcc not found under CROSS_COMPILER_BIN=${CROSS_COMPILER_BIN}. "
            "Check the CFS/MAXIM_LIBRARIES location, or install the ARM GCC toolchain. "
            "To use a compiler already on PATH instead, configure with -DUSE_VENDOR_TOOLCHAIN=OFF.")
    endif()
else()
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
    set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
    set(CMAKE_LINKER arm-none-eabi-ld)
    set(CMAKE_SIZE arm-none-eabi-size)
    set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
endif()

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# Common flags for all build types
set(COMMON_CPU_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16")
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

# Linker flags (common for all build types)
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_CPU_FLAGS} -specs=nosys.specs -Wl,--gc-sections,--undefined=_sbrk ${MCU_LINKER_FLAGS} \
    -T${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC/${TARGET}.ld --entry=Reset_Handler" CACHE STRING "Linker flags for MCU" FORCE)

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

    set(OPENOCD_INTERFACE "interface/cmsis-dap.cfg")
    set(OPENOCD_CHIPNAME ${TARGET})
    set(OPENOCD_TARGETCFG "target/${TARGET}.cfg")

    if (NOT PROBE)
        set(PROBE "openocd")
    endif()

    message(STATUS "Found OpenOCD: ${OPENOCD_PATH}")
else()
    message(STATUS "OpenOCD not found in ${OPENOCD_SEARCH_PATH}")
endif()
