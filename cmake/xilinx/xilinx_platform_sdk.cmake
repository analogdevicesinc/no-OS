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

    # Regenerate only when the BSP artifacts are missing (BSP generation is slow;
    # the .xsa rarely changes within a working tree).
    if(NOT EXISTS "${_bsp_lib}/libxil.a" OR NOT EXISTS "${_lscript}")
        file(MAKE_DIRECTORY "${_ws}")
        file(COPY "${HARDWARE}" DESTINATION "${_ws}")
        # create_project reads the CPU from arch.txt (normally written by
        # get_arch); the arch was already resolved in the toolchain file, so
        # stage it directly instead of re-running get_arch.
        file(WRITE "${_ws}/arch.txt" "${XILINX_ARCH}")

        find_program(VITIS_EXECUTABLE vitis HINTS "$ENV{XILINX_VITIS}/bin")
        if(NOT VITIS_EXECUTABLE)
            message(FATAL_ERROR "vitis not found under $ENV{XILINX_VITIS}/bin")
        endif()

        message(STATUS "Generating Xilinx BSP + linker script from ${_xsa_file} "
                       "(arch ${XILINX_ARCH})... this can take a few minutes")
        execute_process(
            COMMAND ${VITIS_EXECUTABLE} -s
                    "${NO_OS_DIR}/tools/scripts/platform/xilinx/util.py"
                    create_project "${_ws}" "${_ws}" "${_xsa_file}"
            RESULT_VARIABLE _rc
            OUTPUT_VARIABLE _out
            ERROR_VARIABLE _err)
        if(NOT _rc EQUAL 0 OR NOT EXISTS "${_bsp_lib}/libxil.a")
            message(FATAL_ERROR
                "Xilinx BSP generation failed (rc=${_rc}).\n"
                "stdout:\n${_out}\nstderr:\n${_err}")
        endif()
    endif()

    if(NOT EXISTS "${_lscript}")
        message(FATAL_ERROR "Xilinx linker script not found: ${_lscript}")
    endif()

    # BSP headers/libs are shared -> attach to the no-os library (PUBLIC so the
    # executable inherits the include path for xparameters.h etc.).
    target_include_directories(no-os PUBLIC "${_bsp_inc}")
    target_link_directories(${BUILD_TARGET} PUBLIC "${_bsp_lib}")

    # Linker script + Xilinx BSP libraries on the final executable.
    target_link_options(${BUILD_TARGET} PRIVATE -T${_lscript} -Wl,-build-id=none)
    if("${XILINX_ARCH}" MATCHES "cortexa9")
        target_link_options(${BUILD_TARGET} PRIVATE
            -specs=${_ws}/app/src/Xilinx.spec)
    endif()
    # Vitis 2025 splits the standalone BSP into libxil.a plus libxilstandalone.a
    # (syscall stubs: _exit/_sbrk/read/write...) and libxiltimer.a. Older layouts
    # bundle everything into libxil.a, so link the extras only when present.
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
