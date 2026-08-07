# Altera/Nios V platform SDK integration for the CMake build.
#
# This is a compatibility layer for Intel FPGA / Nios V BSP-based builds.
# It is intentionally opt-in and does not alter the existing legacy Altera
# platform behavior. The implementation assumes the BSP is generated externally
# and that the user points no-OS to it via environment variables.
#
# Expected environment variables:
#   ALTERA_BSP_DIR      - path to the generated BSP directory (contains system.h,
#                         linker.h, linker.x, HAL/, drivers/, etc.)
#   ALTERA_SYSROOT      - optional sysroot root for the BSP toolchain
#   ALTERA_CFLAGS       - optional extra C flags for the toolchain
#   ALTERA_LDFLAGS      - optional extra linker flags
#   ALTERA_CPU_NAME     - optional CPU name used in BSP selection/debug messages
#
# The goal is to provide the include paths and build flags needed for a no-OS
# application to compile against an external Altera/Nios V BSP, while leaving
# the existing in-tree Altera sources available for legacy use.

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

function(config_altera_sdk BUILD_TARGET)
    if(NOT DEFINED ENV{ALTERA_BSP_DIR} OR "$ENV{ALTERA_BSP_DIR}" STREQUAL "")
        message(FATAL_ERROR
            "ALTERA_BSP_DIR is not set. Export it to the generated Nios V BSP directory before configuring the build.")
    endif()

    if(NOT DEFINED CMAKE_C_COMPILER OR CMAKE_C_COMPILER STREQUAL "")
        message(FATAL_ERROR "CMAKE_C_COMPILER is not set. Configure the toolchain with the standard CMake compiler variable before project().")
    endif()

    if(NOT DEFINED CMAKE_CXX_COMPILER OR CMAKE_CXX_COMPILER STREQUAL "")
        message(FATAL_ERROR "CMAKE_CXX_COMPILER is not set. Configure the toolchain with the standard CMake compiler variable before project().")
    endif()

    if(NOT DEFINED CMAKE_ASM_COMPILER OR CMAKE_ASM_COMPILER STREQUAL "")
        message(FATAL_ERROR "CMAKE_ASM_COMPILER is not set. Configure the toolchain with the standard CMake compiler variable before project().")
    endif()

    set(_toolchain_cc "${CMAKE_C_COMPILER}")
    if(NOT EXISTS "${_toolchain_cc}")
        message(FATAL_ERROR "Compiler tool does not exist: ${_toolchain_cc}")
    endif()

    set(_bsp_dir "$ENV{ALTERA_BSP_DIR}")
    set(_cpu_name "$ENV{ALTERA_CPU_NAME}")
    if(_cpu_name STREQUAL "")
        set(_cpu_name "niosv")
    endif()

    if(NOT EXISTS "${_bsp_dir}/system.h")
        message(FATAL_ERROR "Generated BSP directory does not look valid: ${_bsp_dir} (missing system.h)")
    endif()

    # The BSP directory usually contains the generated HAL and driver headers.
    set(_bsp_inc
        "${_bsp_dir}"
        "${_bsp_dir}/HAL/inc"
        "${_bsp_dir}/drivers/inc"
    )
    set(_bsp_linker_script "${_bsp_dir}/linker.x")

    set(_project_platform_inc
        "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/platform"
    )

    target_include_directories(no-os PUBLIC ${_bsp_inc} ${_project_platform_inc})
    target_include_directories(${BUILD_TARGET} PUBLIC ${_bsp_inc} ${_project_platform_inc})

    if(EXISTS "${_bsp_dir}/../app_pio/build/bsp/libhal2_bsp.a")
        set(_bsp_archive "${_bsp_dir}/../app_pio/build/bsp/libhal2_bsp.a")
    elseif(EXISTS "${_bsp_dir}/../app_hello/build/bsp/libhal2_bsp.a")
        set(_bsp_archive "${_bsp_dir}/../app_hello/build/bsp/libhal2_bsp.a")
    else()
        set(_bsp_archive "")
    endif()

    target_compile_definitions(${BUILD_TARGET} PRIVATE ALT_SINGLE_THREADED __hal__)
    target_link_options(${BUILD_TARGET} PRIVATE -nostdlib "-T${_bsp_linker_script}")
    if(_bsp_archive)
        target_link_libraries(${BUILD_TARGET} PUBLIC
            -Wl,--start-group
            "${_bsp_archive}"
            -lc
            -lstdc++
            -lgcc
            -lm
            -Wl,--end-group
        )
    else()
        target_link_libraries(${BUILD_TARGET} PUBLIC
            -Wl,--start-group
            -lc
            -lstdc++
            -lgcc
            -lm
            -Wl,--end-group
        )
    endif()

    # Make the BSP headers visible to the generated configuration target too.
    target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/build)

    # Expose the compiler path and BSP path to downstream CMake logic.
    set(ALTERA_BSP_DIR "${_bsp_dir}" CACHE PATH "Generated Intel FPGA / Nios V BSP directory" FORCE)
    set(ALTERA_CPU_NAME "${_cpu_name}" CACHE STRING "Intel FPGA / Nios V CPU name" FORCE)

    # Default toolchain settings if the toolchain file has not already configured them.
    _altera_resolve_tool_path(_toolchain_cxx "g++")
    _altera_resolve_tool_path(_toolchain_asm "gcc")
    _altera_resolve_tool_path(_toolchain_linker "ld")
    _altera_resolve_tool_path(_toolchain_size "size")
    _altera_resolve_tool_path(_toolchain_objcopy "objcopy")
    _altera_resolve_tool_path(_toolchain_objdump "objdump")

    if(NOT DEFINED CMAKE_C_COMPILER)
        set(CMAKE_C_COMPILER "${_toolchain_cc}" CACHE FILEPATH "C compiler" FORCE)
    endif()
    if(NOT DEFINED CMAKE_CXX_COMPILER)
        set(CMAKE_CXX_COMPILER "${_toolchain_cxx}" CACHE FILEPATH "C++ compiler" FORCE)
    endif()
    if(NOT DEFINED CMAKE_ASM_COMPILER)
        set(CMAKE_ASM_COMPILER "${_toolchain_asm}" CACHE FILEPATH "ASM compiler" FORCE)
    endif()
    if(_toolchain_linker)
        set(CMAKE_LINKER "${_toolchain_linker}" CACHE FILEPATH "Linker" FORCE)
    endif()
    if(_toolchain_size)
        set(CMAKE_SIZE "${_toolchain_size}" CACHE FILEPATH "Size tool" FORCE)
    endif()
    if(_toolchain_objcopy)
        set(CMAKE_OBJCOPY "${_toolchain_objcopy}" CACHE FILEPATH "Objcopy" FORCE)
    endif()
    if(_toolchain_objdump)
        set(CMAKE_OBJDUMP "${_toolchain_objdump}" CACHE FILEPATH "Objdump" FORCE)
    endif()

    if(DEFINED ENV{ALTERA_SYSROOT} AND NOT "$ENV{ALTERA_SYSROOT}" STREQUAL "")
        set(CMAKE_SYSROOT "$ENV{ALTERA_SYSROOT}" CACHE PATH "Toolchain sysroot" FORCE)
    endif()

    set(_extra_c_flags "")
    if(DEFINED ENV{ALTERA_CFLAGS} AND NOT "$ENV{ALTERA_CFLAGS}" STREQUAL "")
        set(_extra_c_flags "$ENV{ALTERA_CFLAGS}")
    endif()

    set(_extra_ld_flags "")
    if(DEFINED ENV{ALTERA_LDFLAGS} AND NOT "$ENV{ALTERA_LDFLAGS}" STREQUAL "")
        set(_extra_ld_flags "$ENV{ALTERA_LDFLAGS}")
    endif()

    if(_extra_c_flags)
        target_compile_options(${BUILD_TARGET} PRIVATE ${_extra_c_flags})
    endif()
    if(_extra_ld_flags)
        target_link_options(${BUILD_TARGET} PRIVATE ${_extra_ld_flags})
    endif()

    message(STATUS "Configured Intel FPGA / Nios V BSP compatibility layer for ${_cpu_name}")
    message(STATUS "  BSP: ${_bsp_dir}")
    message(STATUS "  Toolchain compiler: ${_toolchain_cc}")
endfunction()
