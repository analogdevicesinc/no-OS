# Xilinx 'flash' target for the CMake build.
#
# Replicates the legacy `make run` for Xilinx: program the FPGA bitstream and
# download+run the application ELF over JTAG. All work is driven by the Vitis
# Python API in tools/scripts/platform/xilinx/util.py, invoked via `vitis -s`.
#
# The run flow mirrors the old xilinx.mk `xilinx_run` chain:
#   1. Stage a run work dir with a copy of the .xsa and arch.txt.
#   2. For cortexa53/cortexr5, generate the FSBL (util.py create_fsbl).
#   3. Upload: reset, program the bitstream (extracted from the .xsa), init the
#      PS and download the ELF (util.py upload).
#
# Overridable cache variables:
#   TARGET_CPU     - JTAG target CPU selector (util.py `target`); default empty.
#   JTAG_CABLE_ID  - JTAG cable id to disambiguate multiple probes; default empty.

function(add_xilinx_flash_target TARGET_NAME)
    find_program(VITIS_EXECUTABLE vitis HINTS "$ENV{XILINX_VITIS}/bin")
    if(NOT VITIS_EXECUTABLE)
        message(STATUS
            "vitis not found under $ENV{XILINX_VITIS}/bin; 'flash' target "
            "will be unavailable. Source settings64.sh from your Vitis install.")
        return()
    endif()

    set(_util_py "${NO_OS_DIR}/tools/scripts/platform/xilinx/util.py")
    set(_elf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.elf")
    get_filename_component(_xsa_file "${HARDWARE}" NAME)

    # Run work dir. util.py takes `ws` and `hw_path`; matching the legacy layout
    # (ws=<run>, hw_path=<run>/tmp) keeps the FSBL output path consistent with
    # what create_fsbl writes and upload expects.
    set(_run_dir "${CMAKE_CURRENT_BINARY_DIR}/xsa_run")
    set(_hw_path "${_run_dir}/tmp")
    set(_fsbl "${_hw_path}/output/hw0/export/hw0/sw/hw0/boot/fsbl.elf")

    # arch.txt is normally produced by get_arch; we already resolved the arch in
    # the toolchain file (XILINX_ARCH), so stage it directly.
    set(_arch_stage "${CMAKE_CURRENT_BINARY_DIR}/xsa_run_arch.txt")
    file(WRITE "${_arch_stage}" "${XILINX_ARCH}\n")

    # Optional JTAG selectors (empty by default, like the legacy defaults).
    if(NOT DEFINED TARGET_CPU)
        set(TARGET_CPU "")
    endif()
    if(NOT DEFINED JTAG_CABLE_ID)
        set(JTAG_CABLE_ID "")
    endif()
    # util.py normalizes "0"/"" to empty; pass "0" as the placeholder so the
    # positional template/fsbl/jtag args stay aligned when TARGET_CPU is unset.
    if("${TARGET_CPU}" STREQUAL "")
        set(_target_cpu "0")
    else()
        set(_target_cpu "${TARGET_CPU}")
    endif()

    # Stage the run dir: fresh copy of the .xsa + arch.txt on every invocation.
    set(_prep_cmds
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_hw_path}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${HARDWARE}" "${_hw_path}/${_xsa_file}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_arch_stage}" "${_hw_path}/arch.txt")

    # FSBL is required for cortexa53 (ZynqMP) and cortexr5; other arches skip it.
    set(_fsbl_cmd "")
    if("${XILINX_ARCH}" MATCHES "cortexa53" OR "${XILINX_ARCH}" MATCHES "cortexr5")
        set(_fsbl_cmd
            COMMAND ${VITIS_EXECUTABLE} -s "${_util_py}" create_fsbl
                    "${_run_dir}" "${_hw_path}" "${_xsa_file}"
                    "${_elf}" "${_target_cpu}" "Empty Application(C)")
    endif()

    add_custom_target(flash
        ${_prep_cmds}
        ${_fsbl_cmd}
        COMMAND ${VITIS_EXECUTABLE} -s "${_util_py}" upload
                "${_run_dir}" "${_hw_path}" "${_xsa_file}"
                "${_elf}" "${_target_cpu}" "Empty Application(C)"
                "${_fsbl}" "${JTAG_CABLE_ID}"
        DEPENDS ${TARGET_NAME}
        COMMENT "Programming bitstream and running ${TARGET_NAME}.elf on target (Xilinx JTAG)..."
        USES_TERMINAL
        VERBATIM)
endfunction()
