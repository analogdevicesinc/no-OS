# Xilinx platform SDK integration for the CMake build.
#
# Generates a Vitis standalone BSP + linker script from the board .xsa (the
# HARDWARE cache variable) and wires them onto the no-os library and the project
# executable. Analogous to config_stm32_sdk() which runs STM32CubeMX; here we run
# the HSI-based generator in tools/scripts/platform/xilinx/util.py via `vitis -s`.
#
# Products (under ${CMAKE_CURRENT_BINARY_DIR}/xsa_work), created by util.py:
#   bsp/<arch>/include/xparameters.h ...   -> include dir on no-os
#   bsp/<arch>/lib/libxil.a                -> link dir + -lxil on the executable
#   app/src/lscript.ld                     -> -T on the executable
#   app/src/Xilinx.spec (cortexa9 only)    -> -specs= on the executable

function(config_xilinx_sdk BUILD_TARGET)
    set(_ws "${CMAKE_CURRENT_BINARY_DIR}/xsa_work")
    get_filename_component(_xsa_file "${HARDWARE}" NAME)
    set(_bsp_inc "${_ws}/bsp/${XILINX_ARCH}/include")
    set(_bsp_lib "${_ws}/bsp/${XILINX_ARCH}/lib")
    set(_lscript "${_ws}/app/src/lscript.ld")
    set(_stamp "${_ws}/.bsp_stamp")
    set(_util_py "${NO_OS_DIR}/tools/scripts/platform/xilinx/util.py")
    set(_compat_h "${NO_OS_DIR}/drivers/platform/xilinx/xilinx_compat.h")

    # Vitis path is version-stamped, so it also fingerprints the toolchain.
    find_program(VITIS_EXECUTABLE vitis HINTS "$ENV{XILINX_VITIS}/bin")

    # Per-build-dir stamp; regenerate on any input change (cf. STM32 .cubemx_stamp).
    file(SHA256 "${HARDWARE}" _xsa_hash)
    file(SHA256 "${_util_py}" _util_hash)
    file(SHA256 "${_compat_h}" _compat_hash)
    set(_fingerprint
        "${_xsa_hash}:${_util_hash}:${_compat_hash}:${XILINX_ARCH}:${VITIS_EXECUTABLE}")

    set(_need_regen TRUE)
    if(EXISTS "${_bsp_lib}/libxil.a" AND EXISTS "${_lscript}" AND EXISTS "${_stamp}")
        file(READ "${_stamp}" _stored)
        string(STRIP "${_stored}" _stored)
        if("${_stored}" STREQUAL "${_fingerprint}")
            set(_need_regen FALSE)
        else()
            message(STATUS "Xilinx BSP inputs changed, will regenerate")
        endif()
    else()
        message(STATUS "Xilinx BSP missing or unstamped, will regenerate")
    endif()

    if(_need_regen)
        if(NOT VITIS_EXECUTABLE)
            message(FATAL_ERROR "vitis not found under $ENV{XILINX_VITIS}/bin")
        endif()
        if(EXISTS "${_ws}")
            file(REMOVE_RECURSE "${_ws}")
        endif()
        file(MAKE_DIRECTORY "${_ws}")
        file(COPY "${HARDWARE}" DESTINATION "${_ws}")
        # Stage arch.txt so create_project skips the get_arch step.
        file(WRITE "${_ws}/arch.txt" "${XILINX_ARCH}")

        message(STATUS "Generating Xilinx BSP + linker script from ${_xsa_file} "
                       "(arch ${XILINX_ARCH})... this can take a few minutes")
        # Bound it: with no JTAG, vitis can stall forever on XSDB startup.
        execute_process(
            COMMAND ${VITIS_EXECUTABLE} -s
                    "${_util_py}"
                    create_project "${_ws}" "${_ws}" "${_xsa_file}"
            RESULT_VARIABLE _rc
            OUTPUT_VARIABLE _out
            ERROR_VARIABLE _err
            TIMEOUT 900)
        if(NOT _rc EQUAL 0 OR NOT EXISTS "${_bsp_lib}/libxil.a")
            message(FATAL_ERROR
                "Xilinx BSP generation failed (rc=${_rc}). vitis may have hung on "
                "XSDB startup (no JTAG / TCF stall); see "
                "${_ws}/tmp/output/_ide/logs/vitis.log.\n"
                "stdout:\n${_out}\nstderr:\n${_err}")
        endif()
        # Stamp only on success; a failed run is retried next configure.
        file(WRITE "${_stamp}" "${_fingerprint}")
    endif()

    if(NOT EXISTS "${_lscript}")
        message(FATAL_ERROR "Xilinx linker script not found: ${_lscript}")
    endif()

    # BSP headers/libs: PUBLIC on no-os so the executable inherits them.
    target_include_directories(no-os PUBLIC "${_bsp_inc}")
    target_link_directories(${BUILD_TARGET} PUBLIC "${_bsp_lib}")

    # Linker script + Xilinx BSP libraries on the final executable.
    target_link_options(${BUILD_TARGET} PRIVATE -T${_lscript} -Wl,-build-id=none)
    if("${XILINX_ARCH}" MATCHES "cortexa9")
        target_link_options(${BUILD_TARGET} PRIVATE
            -specs=${_ws}/app/src/Xilinx.spec)
    endif()
    # Vitis 2025 split libxil.a; link libxilstandalone/libxiltimer if present.
    set(_bsp_libs -lxil)
    if(EXISTS "${_bsp_lib}/libxilstandalone.a")
        list(APPEND _bsp_libs -lxilstandalone)
    endif()
    if(EXISTS "${_bsp_lib}/libxiltimer.a")
        list(APPEND _bsp_libs -lxiltimer)
    endif()
    target_link_libraries(${BUILD_TARGET}
        -Wl,--start-group ${_bsp_libs} -lgcc -lc -lm -Wl,--end-group)
endfunction()
