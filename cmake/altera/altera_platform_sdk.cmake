# Altera/Nios V platform SDK integration for the CMake build.
#
# This is a compatibility layer for Intel FPGA / Nios V BSP-based builds.
# It is intentionally opt-in and does not alter the existing legacy Altera
# platform behavior. When no BSP path is provided explicitly, the integration
# will try to generate a BSP from the local Nios V example design using the
# Intel `niosv-bsp` tool.
#
# Required environment variables:
#   ALTERA_NIOSV_REPO_DIR - path to the Intel Nios V example design
#
# Optional environment variables:
#   ALTERA_BSP_DIR        - override: path to a pre-built BSP (skips generation)
#   ALTERA_NIOSV_HOME     - niosv install directory (niosv-bsp found inside)
#   ALTERA_NIOSV_BSP      - legacy: direct path to the niosv-bsp executable
#   ALTERA_NIOSV_QPF      - project file relative to REPO_DIR (default: hw/top.qpf)
#   ALTERA_NIOSV_VDS      - system file relative to REPO_DIR
#                           (default: hw/src/vds/qsys_top/qsys_top.vds)
#   ALTERA_CPU_NAME       - optional CPU name for debug messages

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

function(_altera_resolve_bsp_dir OUT_VAR)
    # Allow direct BSP override (pre-built BSP, no generation needed) from
    # either a CMake variable (e.g. set in a CMakePreset or via -D) or the
    # environment. The CMake variable takes precedence so a preset can wire the
    # BSP path without exporting anything.
    set(_bsp_override "")
    if(DEFINED ALTERA_BSP_DIR AND NOT "${ALTERA_BSP_DIR}" STREQUAL "")
        set(_bsp_override "${ALTERA_BSP_DIR}")
    elseif(DEFINED ENV{ALTERA_BSP_DIR} AND NOT "$ENV{ALTERA_BSP_DIR}" STREQUAL "")
        set(_bsp_override "$ENV{ALTERA_BSP_DIR}")
    endif()
    if(NOT "${_bsp_override}" STREQUAL "")
        set(_bsp_dir "${_bsp_override}")
        if(EXISTS "${_bsp_dir}/system.h")
            set(${OUT_VAR} "${_bsp_dir}" PARENT_SCOPE)
            return()
        endif()
        message(WARNING "ALTERA_BSP_DIR is set to ${_bsp_dir} but system.h not found. Falling through to auto-generation.")
    endif()

    set(_repo_dir "$ENV{ALTERA_NIOSV_REPO_DIR}")
    if(_repo_dir STREQUAL "")
        message(FATAL_ERROR "ALTERA_NIOSV_REPO_DIR is not set. Export the path to the Intel Nios V example design repository before configuring the build.")
    endif()

    if(NOT EXISTS "${_repo_dir}")
        message(FATAL_ERROR "Nios V design repository not found: ${_repo_dir}")
    endif()

    set(_bsp_dir "${_repo_dir}/sw/bsp")
    if(EXISTS "${_bsp_dir}/system.h")
        set(${OUT_VAR} "${_bsp_dir}" PARENT_SCOPE)
        return()
    endif()

    # Locate an existing BSP settings file to reuse (e.g. from a previous
    # generation, or preserved alongside the design under bsp_orig/bsp_old).
    # If none exists, niosv-bsp itself can create a fresh settings.bsp from
    # scratch given just the hardware design (-p/-s) below, so this is not
    # a fatal condition.
    set(_settings_bsp "${_bsp_dir}/settings.bsp")
    foreach(_settings_candidate IN ITEMS
        "${_bsp_dir}/settings.bsp"
        "${_repo_dir}/sw/bsp_orig/settings.bsp"
        "${_repo_dir}/sw/bsp_old/settings.bsp"
    )
        if(EXISTS "${_settings_candidate}")
            set(_settings_bsp "${_settings_candidate}")
            break()
        endif()
    endforeach()

    file(MAKE_DIRECTORY "${_bsp_dir}")
    if(NOT EXISTS "${_bsp_dir}/settings.bsp" AND NOT "${_settings_bsp}" STREQUAL "${_bsp_dir}/settings.bsp")
        file(COPY "${_settings_bsp}" DESTINATION "${_bsp_dir}")
        set(_settings_bsp "${_bsp_dir}/settings.bsp")
    endif()

    # Locate niosv-bsp tool: ALTERA_NIOSV_HOME/bin → ALTERA_NIOSV_BSP → PATH
    # NO_CACHE is required here: find_program() caches its result by variable
    # name, and a cached hit/miss from an earlier configure (e.g. while the
    # env vars below were still being set up) would otherwise be reused
    # silently on later configures instead of being re-searched.
    # Note: the variable must stay *unset* (not set to "") going into the
    # first find_program() call below — find_program() treats a variable
    # already holding a defined value (even an empty string) as "already
    # resolved" and skips the search entirely, regardless of NO_CACHE.
    unset(_niosv_bsp_executable)
    if(DEFINED ENV{ALTERA_NIOSV_HOME} AND NOT "$ENV{ALTERA_NIOSV_HOME}" STREQUAL "")
        find_program(_niosv_bsp_executable NAMES niosv-bsp
            HINTS "$ENV{ALTERA_NIOSV_HOME}/bin" NO_DEFAULT_PATH NO_CACHE)
    endif()
    if(NOT _niosv_bsp_executable AND DEFINED ENV{ALTERA_NIOSV_BSP} AND NOT "$ENV{ALTERA_NIOSV_BSP}" STREQUAL "")
        set(_niosv_bsp_executable "$ENV{ALTERA_NIOSV_BSP}")
    endif()
    if(NOT _niosv_bsp_executable)
        find_program(_niosv_bsp_executable NAMES niosv-bsp NO_CACHE)
    endif()
    if(NOT _niosv_bsp_executable)
        message(FATAL_ERROR "Unable to find niosv-bsp. Set ALTERA_NIOSV_HOME or ALTERA_NIOSV_BSP.")
    endif()

    # Resolve design file paths (configurable for non-standard layouts)
    if(DEFINED ENV{ALTERA_NIOSV_QPF} AND NOT "$ENV{ALTERA_NIOSV_QPF}" STREQUAL "")
        set(_qpf "$ENV{ALTERA_NIOSV_QPF}")
    else()
        set(_qpf "hw/top.qpf")
    endif()
    if(DEFINED ENV{ALTERA_NIOSV_VDS} AND NOT "$ENV{ALTERA_NIOSV_VDS}" STREQUAL "")
        set(_vds "$ENV{ALTERA_NIOSV_VDS}")
    else()
        set(_vds "hw/src/vds/qsys_top/qsys_top.vds")
    endif()

    message(STATUS "Generating Intel FPGA / Nios V BSP in ${_bsp_dir}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E chdir "${_repo_dir}"
            "${_niosv_bsp_executable}" -c -t=hal
            -p=${_qpf} -s=${_vds} "${_settings_bsp}"
        RESULT_VARIABLE _bsp_result
        OUTPUT_VARIABLE _bsp_stdout
        ERROR_VARIABLE _bsp_stderr
        TIMEOUT 120
    )
    if(NOT _bsp_result EQUAL 0)
        message(FATAL_ERROR "Failed to generate Nios V BSP from ${_repo_dir}\n"
            "stdout:\n${_bsp_stdout}\nstderr:\n${_bsp_stderr}")
    endif()

    if(NOT EXISTS "${_bsp_dir}/system.h")
        message(FATAL_ERROR "Nios V BSP generation completed but ${_bsp_dir}/system.h was not produced.")
    endif()

    set(${OUT_VAR} "${_bsp_dir}" PARENT_SCOPE)
endfunction()

function(config_altera_sdk BUILD_TARGET)
    set(_toolchain_cc "${CMAKE_C_COMPILER}")
    if(NOT EXISTS "${_toolchain_cc}")
        message(FATAL_ERROR "Compiler tool does not exist: ${_toolchain_cc}")
    endif()

    _altera_resolve_bsp_dir(_bsp_dir)
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

    # Locate the pre-built BSP archive.
    set(_bsp_archive "")
    if(DEFINED ALTERA_BSP_LIB AND EXISTS "${ALTERA_BSP_LIB}")
        set(_bsp_archive "${ALTERA_BSP_LIB}")
    elseif(DEFINED ENV{ALTERA_BSP_LIB} AND EXISTS "$ENV{ALTERA_BSP_LIB}")
        set(_bsp_archive "$ENV{ALTERA_BSP_LIB}")
    elseif(EXISTS "${_bsp_dir}/../app_pio/build/bsp/libhal2_bsp.a")
        set(_bsp_archive "${_bsp_dir}/../app_pio/build/bsp/libhal2_bsp.a")
    elseif(EXISTS "${_bsp_dir}/../app_hello/build/bsp/libhal2_bsp.a")
        set(_bsp_archive "${_bsp_dir}/../app_hello/build/bsp/libhal2_bsp.a")
    elseif(EXISTS "${_bsp_dir}/libhal2_bsp.a")
        set(_bsp_archive "${_bsp_dir}/libhal2_bsp.a")
    endif()

    if(NOT _bsp_archive)
        message(WARNING "Nios V BSP archive (libhal2_bsp.a) not found. "
            "Link may fail. Set ALTERA_BSP_LIB if it is in a non-standard location.")
    endif()

    target_compile_definitions(${BUILD_TARGET} PRIVATE ALT_SINGLE_THREADED __hal__)
    target_link_options(${BUILD_TARGET} PRIVATE -nostdlib "-T${_bsp_linker_script}")
    # Plain signature to match the project's target_link_libraries(<exe> no-os);
    # mixing keyword and plain forms on one target is a CMake error.
    if(_bsp_archive)
        target_link_libraries(${BUILD_TARGET}
            -Wl,--start-group
            "${_bsp_archive}"
            -lc
            -lstdc++
            -lgcc
            -lm
            -Wl,--end-group
        )
    else()
        target_link_libraries(${BUILD_TARGET}
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

    set(ALTERA_CPU_NAME "${_cpu_name}" CACHE STRING "Intel FPGA / Nios V CPU name" FORCE)

    # Resolve auxiliary toolchain tools from the compiler path.
    _altera_resolve_tool_path(_toolchain_cxx "g++")
    _altera_resolve_tool_path(_toolchain_linker "ld")
    _altera_resolve_tool_path(_toolchain_size "size")
    _altera_resolve_tool_path(_toolchain_objcopy "objcopy")
    _altera_resolve_tool_path(_toolchain_objdump "objdump")

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

    message(STATUS "Configured Intel FPGA / Nios V BSP for ${_cpu_name}")
    message(STATUS "  BSP: ${_bsp_dir}")
    message(STATUS "  Compiler: ${_toolchain_cc}")
endfunction()
