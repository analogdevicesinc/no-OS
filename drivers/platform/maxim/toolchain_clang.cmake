# Clang-based toolchain for Maxim platforms
# Alternative to the default GCC-based toolchain

# Support both CFS and traditional MAXIM_LIBRARIES setup
find_path(CFS
          NAMES
            cfs.json
          PATHS
            "/mnt/drive/downloads/cfs_2.0.0"
)

if (DEFINED CFS)
    set(MAXIM_LIBRARIES ${CFS}/SDK/MAX/Libraries)
elseif(DEFINED ENV{MAXIM_LIBRARIES})
    set(MAXIM_LIBRARIES $ENV{MAXIM_LIBRARIES})
else()
    message(FATAL_ERROR "MAXIM_LIBRARIES environment variable is not defined and CFS not found")
endif()

message(STATUS "MAXIM_LIBRARIES: ${MAXIM_LIBRARIES}")
message(STATUS "Using Clang toolchain for Maxim platform")
if(DEFINED CFS)
    message(STATUS "Using CFS with CMSIS 6")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Include target-specific memory layout if available
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/max${TARGET_NUM}/memory_layout.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/max${TARGET_NUM}/memory_layout.cmake")
endif()

# Set up Clang compiler paths with graceful fallback to GCC
if(1)
    if(DEFINED CFS)
        cmake_path(SET CROSS_COMPILER_BIN NORMALIZE "/mnt/drive/ATfE-21.1.1-Linux-x86_64/bin")
    else()
        message(FATAL_ERROR "CFS installation not found!")
    endif()

    # Try to find clang in vendor toolchain first (if available)
    # Search for versioned clang (clang-22, clang-21, etc.) and generic names
    find_program(CLANG_C_COMPILER NAMES
        clang-22 clang-21 clang-20 clang-19 clang-18 clang-17 clang-16 clang-15 clang-14 clang-13 clang-12
        clang clang_wrapper
        HINTS ${CROSS_COMPILER_BIN})
    find_program(CLANG_CXX_COMPILER NAMES
        clang++-22 clang++-21 clang++-20 clang++-19 clang++-18 clang++-17 clang++-16 clang++-15 clang++-14 clang++-13 clang++-12
        clang++ clang++_wrapper
        HINTS ${CROSS_COMPILER_BIN})
    find_program(CLANG_ASM_COMPILER NAMES
        clang-22 clang-21 clang-20 clang-19 clang-18 clang-17 clang-16 clang-15 clang-14 clang-13 clang-12
        clang clang_wrapper
        HINTS ${CROSS_COMPILER_BIN})

    # If not found in vendor tools, try system-wide
    if(NOT CLANG_C_COMPILER)
        find_program(CLANG_C_COMPILER NAMES
            clang-22 clang-21 clang-20 clang-19 clang-18 clang-17 clang-16 clang-15 clang-14 clang-13 clang-12
            clang clang_wrapper)
    endif()
    if(NOT CLANG_CXX_COMPILER)
        find_program(CLANG_CXX_COMPILER NAMES
            clang++-22 clang++-21 clang++-20 clang++-19 clang++-18 clang++-17 clang++-16 clang++-15 clang++-14 clang++-13 clang++-12
            clang++ clang++_wrapper)
    endif()
    if(NOT CLANG_ASM_COMPILER)
        find_program(CLANG_ASM_COMPILER NAMES
            clang-22 clang-21 clang-20 clang-19 clang-18 clang-17 clang-16 clang-15 clang-14 clang-13 clang-12
            clang clang_wrapper)
    endif()

    # If clang is found, use it; otherwise fallback to GCC
    if(CLANG_C_COMPILER AND CLANG_CXX_COMPILER AND CLANG_ASM_COMPILER)
        set(CMAKE_C_COMPILER ${CLANG_C_COMPILER})
        set(CMAKE_CXX_COMPILER ${CLANG_CXX_COMPILER})
        set(CMAKE_ASM_COMPILER ${CLANG_ASM_COMPILER})
        set(USING_CLANG TRUE)
        message(STATUS "Using Clang compilers from: ${CMAKE_C_COMPILER}")
    else()
        message(FATAL_ERROR "Clang not found")
    endif()

    # Use LLVM tools if clang is available, otherwise fallback to binutils
    if(USING_CLANG)
        find_program(CMAKE_LINKER NAMES
            ld.lld-22 ld.lld-21 ld.lld-20 ld.lld-19 ld.lld-18 ld.lld-17 ld.lld-16 ld.lld-15 ld.lld-14 ld.lld-13 ld.lld-12
            ld.lld lld
            HINTS ${CROSS_COMPILER_BIN})
        find_program(CMAKE_AR NAMES
            llvm-ar-22 llvm-ar-21 llvm-ar-20 llvm-ar-19 llvm-ar-18 llvm-ar-17 llvm-ar-16 llvm-ar-15 llvm-ar-14 llvm-ar-13 llvm-ar-12
            llvm-ar
            HINTS ${CROSS_COMPILER_BIN})
        find_program(CMAKE_RANLIB NAMES
            llvm-ranlib-22 llvm-ranlib-21 llvm-ranlib-20 llvm-ranlib-19 llvm-ranlib-18 llvm-ranlib-17 llvm-ranlib-16 llvm-ranlib-15 llvm-ranlib-14 llvm-ranlib-13 llvm-ranlib-12
            llvm-ranlib
            HINTS ${CROSS_COMPILER_BIN})
        find_program(CMAKE_OBJCOPY NAMES
            llvm-objcopy-22 llvm-objcopy-21 llvm-objcopy-20 llvm-objcopy-19 llvm-objcopy-18 llvm-objcopy-17 llvm-objcopy-16 llvm-objcopy-15 llvm-objcopy-14 llvm-objcopy-13 llvm-objcopy-12
            llvm-objcopy
            HINTS ${CROSS_COMPILER_BIN})
        find_program(CMAKE_SIZE NAMES
            llvm-size-22 llvm-size-21 llvm-size-20 llvm-size-19 llvm-size-18 llvm-size-17 llvm-size-16 llvm-size-15 llvm-size-14 llvm-size-13 llvm-size-12
            llvm-size
            HINTS ${CROSS_COMPILER_BIN})
    endif()
endif()

# else()
#     # Use system-wide installation
#     # Search for versioned clang (clang-22, clang-21, etc.) and generic names
#     find_program(CLANG_C_COMPILER NAMES
#         clang-22 clang-21 clang-20 clang-19 clang-18 clang-17 clang-16 clang-15 clang-14 clang-13 clang-12
#         clang clang_wrapper)
#     find_program(CLANG_CXX_COMPILER NAMES
#         clang++-22 clang++-21 clang++-20 clang++-19 clang++-18 clang++-17 clang++-16 clang++-15 clang++-14 clang++-13 clang++-12
#         clang++ clang++_wrapper)
#     find_program(CLANG_ASM_COMPILER NAMES
#         clang-22 clang-21 clang-20 clang-19 clang-18 clang-17 clang-16 clang-15 clang-14 clang-13 clang-12
#         clang clang_wrapper)

#     # If clang is found, use it; otherwise fallback to GCC
#     if(CLANG_C_COMPILER AND CLANG_CXX_COMPILER AND CLANG_ASM_COMPILER)
#         set(CMAKE_C_COMPILER ${CLANG_C_COMPILER})
#         set(CMAKE_CXX_COMPILER ${CLANG_CXX_COMPILER})
#         set(CMAKE_ASM_COMPILER ${CLANG_ASM_COMPILER})
#         set(USING_CLANG TRUE)
#         message(STATUS "Using system Clang compilers")
#     else()
#         message(WARNING "Clang not found in system PATH, falling back to GCC")
#         set(CMAKE_C_COMPILER arm-none-eabi-gcc)
#         set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
#         set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
#         set(USING_CLANG FALSE)
#     endif()

#     # Prefer LLVM tools if using clang, fallback to GNU binutils
#     if(USING_CLANG)
#         find_program(CMAKE_LINKER NAMES
#             ld.lld-22 ld.lld-21 ld.lld-20 ld.lld-19 ld.lld-18 ld.lld-17 ld.lld-16 ld.lld-15 ld.lld-14 ld.lld-13 ld.lld-12
#             ld.lld lld)
#         find_program(CMAKE_AR NAMES
#             llvm-ar-22 llvm-ar-21 llvm-ar-20 llvm-ar-19 llvm-ar-18 llvm-ar-17 llvm-ar-16 llvm-ar-15 llvm-ar-14 llvm-ar-13 llvm-ar-12
#             llvm-ar)
#         find_program(CMAKE_RANLIB NAMES
#             llvm-ranlib-22 llvm-ranlib-21 llvm-ranlib-20 llvm-ranlib-19 llvm-ranlib-18 llvm-ranlib-17 llvm-ranlib-16 llvm-ranlib-15 llvm-ranlib-14 llvm-ranlib-13 llvm-ranlib-12
#             llvm-ranlib)
#         find_program(CMAKE_OBJCOPY NAMES
#             llvm-objcopy-22 llvm-objcopy-21 llvm-objcopy-20 llvm-objcopy-19 llvm-objcopy-18 llvm-objcopy-17 llvm-objcopy-16 llvm-objcopy-15 llvm-objcopy-14 llvm-objcopy-13 llvm-objcopy-12
#             llvm-objcopy)
#         find_program(CMAKE_SIZE NAMES
#             llvm-size-22 llvm-size-21 llvm-size-20 llvm-size-19 llvm-size-18 llvm-size-17 llvm-size-16 llvm-size-15 llvm-size-14 llvm-size-13 llvm-size-12
#             llvm-size)
#     endif()

#     if(NOT CMAKE_LINKER)
#         set(CMAKE_LINKER arm-none-eabi-ld)
#     endif()
#     if(NOT CMAKE_AR)
#         set(CMAKE_AR arm-none-eabi-ar)
#     endif()
#     if(NOT CMAKE_RANLIB)
#         set(CMAKE_RANLIB arm-none-eabi-ranlib)
#     endif()
#     if(NOT CMAKE_OBJCOPY)
#         set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
#     endif()
#     if(NOT CMAKE_SIZE)
#         set(CMAKE_SIZE arm-none-eabi-size)
#     endif()
# endif()

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# Configure compiler-specific flags based on what compiler is being used
if(1)
    # Clang-specific target triple for ARM Cortex-M4
    set(CLANG_TARGET_TRIPLE "arm-none-eabi")

    # Common flags for Clang ARM Cortex-M4 compilation
    set(COMMON_FLAGS
        "--target=${CLANG_TARGET_TRIPLE}"
        "-mthumb"
        "-mcpu=cortex-m4"
        "-mfloat-abi=hard"
        "-mfpu=fpv4-sp-d16"
        "-ffunction-sections"
        "-fdata-sections"
    )

    # Add ATfE sysroot for C library
    # if(DEFINED CFS)
    #     list(APPEND COMMON_FLAGS "--sysroot=/mnt/drive/ATfE-21.1.1-Linux-x86_64/arm-none-eabi")
    # endif()
endif()

# C compiler flags (common for all build types)
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
list(JOIN CMAKE_C_FLAGS " " CMAKE_C_FLAGS)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MD -Wall -Wextra -Wno-unused-parameter" CACHE STRING "C compiler flags" FORCE)

# C++ compiler flags (common for all build types)
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}")
list(JOIN CMAKE_CXX_FLAGS " " CMAKE_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MD -fno-rtti -fno-exceptions -Wall -Wextra -Wno-unused-parameter" CACHE STRING "C++ compiler flags" FORCE)

# Assembly flags
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")
list(JOIN CMAKE_ASM_FLAGS " " CMAKE_ASM_FLAGS)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp" CACHE STRING "ASM compiler flags" FORCE)

# Linker flags - handle both ld.lld and GNU ld, and clang vs GCC
# Note: CFS linker scripts are incompatible with lld, so use GNU ld with CFS
if(1)
    # LLVM lld linker flags with clang (non-CFS builds)
    set(CMAKE_EXE_LINKER_FLAGS
        "--target=${CLANG_TARGET_TRIPLE} -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 \
        -Wl,--gc-sections ${MCU_LINKER_FLAGS} \
        -T${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC/${TARGET}.ld \
        -Wl,--entry=Reset_Handler -nostartfiles -lcrt0-semihost -lsemihost"
        CACHE STRING "Linker flags for MCU with Clang and LLD" FORCE)

# elseif(USING_CLANG)
#     # Clang with GNU ld (CFS linker scripts require GNU ld, not lld)
#     set(CMAKE_EXE_LINKER_FLAGS
#         "--target=${CLANG_TARGET_TRIPLE} -mthumb -mcpu=cortex-m4 -nostdlib -nostartfiles \
#         --rtlib=compiler-rt -Wl,--gc-sections -mfpu=fpv4-sp-d16 ${MCU_LINKER_FLAGS} \
#         -T${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC/${TARGET}.ld \
#         -Wl,--entry=Reset_Handler"
#         CACHE STRING "Linker flags for MCU with Clang and GNU LD" FORCE)
# else()
#     # GCC fallback (same as original toolchain)
#     set(CMAKE_EXE_LINKER_FLAGS
#         "-mthumb -mcpu=cortex-m4 -specs=nosys.specs -Wl,--gc-sections -mfpu=fpv4-sp-d16 ${MCU_LINKER_FLAGS} \
#         -T${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC/${TARGET}.ld --entry=Reset_Handler"
#         CACHE STRING "Linker flags for MCU with GCC fallback" FORCE)
endif()

# OpenOCD configuration
if (DEFINED CFS)
    cmake_path(SET OPENOCD_SEARCH_PATH NORMALIZE "${CFS}/Tools/openocd")
    find_program(OPENOCD_PATH
        NAMES openocd
        HINTS ${OPENOCD_SEARCH_PATH}
        PATH_SUFFIXES bin
        DOC "Path to OpenOCD executable"
    )
    if(OPENOCD_PATH)
        cmake_path(SET OPENOCD_SCRIPTS NORMALIZE "${CFS}/Tools/openocd/share/openocd/scripts")
    endif()
else()
    file(GLOB_RECURSE OPENOCD_PATH ${MAXIM_LIBRARIES}/../Tools/*/openocd)
    if(OPENOCD_PATH)
        list(GET OPENOCD_PATH 0 OPENOCD_PATH)
        cmake_path(SET OPENOCD_SCRIPTS NORMALIZE "${MAXIM_LIBRARIES}/../Tools/OpenOCD/scripts")
    endif()
endif()

if(OPENOCD_PATH)
    set(OPENOCD_INTERFACE "interface/cmsis-dap.cfg")
    set(OPENOCD_CHIPNAME ${TARGET})
    set(OPENOCD_TARGETCFG "target/${TARGET}.cfg")

    if (NOT PROBE)
        set(PROBE "openocd")
    endif()
endif()

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

# Clang-specific workarounds for CMSIS compatibility
# CMSIS 6 (used with CFS) supports upstream clang natively
# Only apply compatibility wrapper for older CMSIS versions
# if(USING_CLANG AND NOT DEFINED CFS)
#     # Create a clang-specific CMSIS compatibility wrapper for CMSIS <6
#     # This replaces the problematic inline assembly with clang built-ins
#     file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/clang_cmsis_compat.h"
#     "/* Clang compatibility for CMSIS functions */
# #ifndef __CLANG_CMSIS_COMPAT_H
# #define __CLANG_CMSIS_COMPAT_H

# #ifdef __clang__
# /* Override problematic CMSIS inline assembly functions with clang built-ins */
# #if ((defined (__FPU_PRESENT) && (__FPU_PRESENT == 1U)) && \\
#      (defined (__FPU_USED   ) && (__FPU_USED    == 1U)))

# #undef __set_FPSCR
# #define __set_FPSCR __builtin_arm_set_fpscr

# #undef __get_FPSCR
# #define __get_FPSCR __builtin_arm_get_fpscr

# #endif /* FPU_PRESENT && FPU_USED */

# /* Other clang built-ins for ARM */
# #undef __DSB
# #define __DSB() __builtin_arm_dsb(0xF)

# #undef __DMB
# #define __DMB() __builtin_arm_dmb(0xF)

# #undef __ISB
# #define __ISB() __builtin_arm_isb(0xF)

# #undef __WFI
# #define __WFI() __builtin_arm_wfi()

# #undef __WFE
# #define __WFE() __builtin_arm_wfe()

# #undef __SEV
# #define __SEV() __builtin_arm_sev()

# #endif /* __clang__ */
# #endif /* __CLANG_CMSIS_COMPAT_H */
# ")

#     # Include the compatibility wrapper after CMSIS headers
#     list(APPEND COMMON_FLAGS "-include")
#     list(APPEND COMMON_FLAGS "${CMAKE_CURRENT_BINARY_DIR}/clang_cmsis_compat.h")
# endif()

# Clang static analyzer support (optional)
option(ENABLE_CLANG_STATIC_ANALYZER "Enable Clang Static Analyzer" OFF)
if(ENABLE_CLANG_STATIC_ANALYZER)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --analyze")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --analyze")
endif()

message(STATUS "Clang toolchain configured for ${TARGET}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Linker: ${CMAKE_LINKER}")