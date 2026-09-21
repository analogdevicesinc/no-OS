message(STATUS "Altera / Intel FPGA platform")

# This toolchain file supports both the legacy Altera/Nios II style flow and
# the newer Nios V / Intel FPGA BSP-based flow. The selected mode can come
# from Kconfig (CONFIG_ALTERA_PLATFORM_NIOSV) or from the exported
# ALTERA_PLATFORM_NIOSV environment variable. For Nios V BSP mode, the
# toolchain uses the standard CMake compiler variables directly.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR niosv)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

function(_altera_resolve_tool_path OUT_VAR TOOL_NAME)
    if(NOT DEFINED CMAKE_C_COMPILER OR CMAKE_C_COMPILER STREQUAL "")
        set(${OUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    get_filename_component(_altera_compiler_dir "${CMAKE_C_COMPILER}" DIRECTORY)
    get_filename_component(_altera_compiler_name "${CMAKE_C_COMPILER}" NAME)

    set(_altera_candidate_names "${TOOL_NAME}")
    string(REGEX REPLACE "(-gcc|-g\\+\\+|-cc|-clang|-clang\\+\\+)$" "" _altera_tool_prefix "${_altera_compiler_name}")
    if(_altera_tool_prefix AND NOT _altera_tool_prefix STREQUAL "${_altera_compiler_name}")
        list(APPEND _altera_candidate_names "${_altera_tool_prefix}-${TOOL_NAME}")
    endif()
    if(TOOL_NAME STREQUAL "g++" AND _altera_compiler_name MATCHES "gcc$")
        string(REGEX REPLACE "gcc$" "g++" _altera_prefixed_name "${_altera_compiler_name}")
        list(APPEND _altera_candidate_names "${_altera_prefixed_name}")
    elseif(TOOL_NAME STREQUAL "gcc" AND _altera_compiler_name MATCHES "g\\+\\+$")
        string(REGEX REPLACE "g\\+\\+$" "gcc" _altera_prefixed_name "${_altera_compiler_name}")
        list(APPEND _altera_candidate_names "${_altera_prefixed_name}")
    endif()

    if(_altera_compiler_dir)
        foreach(_altera_candidate_name IN LISTS _altera_candidate_names)
            set(_altera_candidate_path "${_altera_compiler_dir}/${_altera_candidate_name}")
            if(EXISTS "${_altera_candidate_path}")
                set(${OUT_VAR} "${_altera_candidate_path}" PARENT_SCOPE)
                return()
            endif()
        endforeach()
    endif()

    find_program(_altera_resolved NAMES ${_altera_candidate_names} PATHS "${_altera_compiler_dir}" NO_DEFAULT_PATH)
    if(_altera_resolved)
        set(${OUT_VAR} "${_altera_resolved}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

set(_altera_use_niosv OFF)
if(DEFINED CONFIG_ALTERA_PLATFORM_NIOSV AND "${CONFIG_ALTERA_PLATFORM_NIOSV}" STREQUAL "ON")
    set(_altera_use_niosv ON)
elseif(DEFINED ENV{ALTERA_PLATFORM_NIOSV} AND NOT "$ENV{ALTERA_PLATFORM_NIOSV}" STREQUAL "")
    set(_altera_use_niosv ON)
elseif(DEFINED BOARD AND "${BOARD}" STREQUAL "altera")
    set(_altera_use_niosv ON)
endif()

set(_altera_cc "")
if(DEFINED ENV{CMAKE_C_COMPILER} AND NOT "$ENV{CMAKE_C_COMPILER}" STREQUAL "")
    set(_altera_cc "$ENV{CMAKE_C_COMPILER}")
elseif(DEFINED CMAKE_C_COMPILER AND NOT CMAKE_C_COMPILER STREQUAL "")
    if(IS_ABSOLUTE "${CMAKE_C_COMPILER}" OR "${CMAKE_C_COMPILER}" MATCHES "/")
        set(_altera_cc "${CMAKE_C_COMPILER}")
    else()
        message(FATAL_ERROR "Altera toolchain requires a full path in CMAKE_C_COMPILER; got '${CMAKE_C_COMPILER}'.")
    endif()
endif()

if(_altera_use_niosv AND (NOT DEFINED ENV{ALTERA_BSP_DIR} OR "$ENV{ALTERA_BSP_DIR}" STREQUAL "") AND NOT DEFINED ALTERA_BSP_DIR)
    message(STATUS "Altera toolchain: no ALTERA_BSP_DIR was supplied; the BSP will be resolved by the Altera SDK integration during configure")
endif()

if(_altera_cc STREQUAL "")
    message(STATUS "Altera toolchain: CMAKE_C_COMPILER was not supplied; relying on CMake's compiler detection")
else()
    set(CMAKE_C_COMPILER "${_altera_cc}" CACHE FILEPATH "C compiler" FORCE)
endif()

_altera_resolve_tool_path(_altera_linker "ld")
if(_altera_linker)
    set(CMAKE_LINKER "${_altera_linker}" CACHE FILEPATH "Linker" FORCE)
endif()
_altera_resolve_tool_path(_altera_size "size")
if(_altera_size)
    set(CMAKE_SIZE "${_altera_size}" CACHE FILEPATH "Size tool" FORCE)
endif()
_altera_resolve_tool_path(_altera_objcopy "objcopy")
if(_altera_objcopy)
    set(CMAKE_OBJCOPY "${_altera_objcopy}" CACHE FILEPATH "Objcopy" FORCE)
endif()
_altera_resolve_tool_path(_altera_objdump "objdump")
if(_altera_objdump)
    set(CMAKE_OBJDUMP "${_altera_objdump}" CACHE FILEPATH "Objdump" FORCE)
endif()

# BFD names for embedding binary blobs (firmware images, profiles) into the ELF
# via `objcopy -I binary -O <fmt> -B <arch>`. Nios V is a little-endian RV32
# target, so its object format is elf32-littleriscv / architecture riscv
# (confirmed with `riscv32-unknown-elf-objcopy --info`). FirmwareBlob.cmake
# hard-errors if these are unset, so define them for every Altera build.
set(NO_OS_OBJCOPY_BIN_FORMAT "elf32-littleriscv" CACHE INTERNAL
    "objcopy -O target for binary blobs embedded in the ELF")
set(NO_OS_OBJCOPY_BIN_ARCH "riscv" CACHE INTERNAL
    "objcopy -B architecture for binary blobs embedded in the ELF")

# The RISC-V newlib toolchain defines int32_t as `long int` (not `int`), so the
# many no-OS drivers that mix `int` and `int32_t` in function-pointer types trip
# -Wincompatible-pointer-types, which GCC 14+ treats as a hard error by default.
# On the RV32 ABI `int` and `long` are both 32-bit and passed identically, so
# these assignments are call-compatible; downgrade the diagnostic to a warning
# for the Nios V build rather than editing drivers shared by every platform.
# The Nios V/g soft core is an RV32IM part with the Zicbom cache-management
# extension and no hardware FPU (soft-float). Build the application to match the
# core so it uses hardware mul/div and cache-block ops instead of libgcc soft
# routines, and so it matches the BSP archive (also built -march=rv32im_zicbom).
# Scoped to the Nios V branch: the legacy Altera/Nios II flow (and every non-
# Altera platform, which uses its own toolchain file) is unaffected. Folded into
# the full-string FORCE set()s below so repeated toolchain reads stay idempotent.
set(_altera_arch_flags "")
if(_altera_use_niosv)
    set(_altera_arch_flags " -march=rv32im_zicbom -mabi=ilp32")
endif()

set(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections -std=gnu11 -Wall -Wextra -Wno-error=incompatible-pointer-types${_altera_arch_flags}" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "-ffunction-sections -fdata-sections -Wall -Wextra${_altera_arch_flags}" CACHE STRING "C++ compiler flags" FORCE)
set(CMAKE_ASM_FLAGS "-D__ASSEMBLY__${_altera_arch_flags}" CACHE STRING "ASM compiler flags" FORCE)

if(DEFINED ENV{ALTERA_CFLAGS} AND NOT "$ENV{ALTERA_CFLAGS}" STREQUAL "")
    string(APPEND CMAKE_C_FLAGS " $ENV{ALTERA_CFLAGS}")
    string(APPEND CMAKE_CXX_FLAGS " $ENV{ALTERA_CFLAGS}")
    string(APPEND CMAKE_ASM_FLAGS " $ENV{ALTERA_CFLAGS}")
endif()

if(DEFINED ENV{ALTERA_LDFLAGS} AND NOT "$ENV{ALTERA_LDFLAGS}" STREQUAL "")
    set(CMAKE_EXE_LINKER_FLAGS "$ENV{ALTERA_LDFLAGS}" CACHE STRING "Linker flags" FORCE)
endif()

# Match the link-time libgcc multilib to the core (see the -march note above).
# Guarded against re-append on repeated toolchain reads.
if(_altera_use_niosv AND NOT CMAKE_EXE_LINKER_FLAGS MATCHES "march=rv32im")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -march=rv32im_zicbom -mabi=ilp32" CACHE STRING "Linker flags" FORCE)
endif()

message(STATUS "Configured Altera toolchain mode: ${_altera_use_niosv}")
if(_altera_cc STREQUAL "")
    message(STATUS "  prefix: ${_altera_prefix}")
else()
    message(STATUS "  compiler: ${_altera_cc}")
endif()
if(_altera_use_niosv)
    if(DEFINED ALTERA_BSP_DIR AND NOT "${ALTERA_BSP_DIR}" STREQUAL "")
        message(STATUS "  BSP directory: ${ALTERA_BSP_DIR}")
    else()
        message(STATUS "  BSP directory: resolved later by the Altera SDK integration")
    endif()
endif()
