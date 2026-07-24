# --- Vitis IDE Backend (Xilinx) ---
#
# Generates the artifacts the Vitis 2025+ Unified IDE needs to debug the
# project, mirroring the legacy `make sdkopen` (which ran `vitis_launch_config`
# then opened the GUI):
#
#   <ws>/_ide/<xsa>/*.bit|*.pdi|ps7_init.tcl|psu_init.tcl   (extracted from XSA)
#   <ws>/_ide/launch.json                                   (debug config)
#
# where <ws> is the Vitis workspace `--open` opens and populates:
#   <proj_bin>/xsa_work/ide/workspace
# (see open_vitis_workspace / util.py create_ide_workspace). launch.json's
# ${workspaceFolder} resolves to <ws>, so the ELF (build/<t>.elf), FSBL
# (xsa_run/...) and staged XSA (xsa_work/<xsa>) are referenced relative to it.
#
# The heavy lifting (BSP + linker script) happens in config_xilinx_sdk()
# (cmake/xilinx/xilinx_platform_sdk.cmake); this backend only wires up the IDE.

function(ide_vitis_configure PROJECT_TARGET)
    if(NOT PLATFORM STREQUAL "xilinx")
        return()
    endif()

    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(NOT Python3_Interpreter_FOUND)
        message(STATUS "python3 not found; skipping Vitis launch.json generation")
        return()
    endif()
    # The workspace root `--open` opens (create_ide_workspace builds hw0/app
    # here). launch.json's ${workspaceFolder} resolves to this dir, so all paths
    # are computed relative to it.
    set(_ws "${CMAKE_CURRENT_BINARY_DIR}/xsa_work/ide/workspace")
    # Stage the debug artifacts in a sibling dir, NOT the workspace itself: a
    # pre-existing _ide/ makes Vitis reject create_client ("cannot recognize the
    # workspace version"). create_ide_workspace copies staging/_ide into the
    # workspace after Vitis initializes it.
    get_filename_component(_xsa_file "${HARDWARE}" NAME)
    get_filename_component(_xsa_base "${HARDWARE}" NAME_WE)
    set(_ide_dir "${CMAKE_CURRENT_BINARY_DIR}/xsa_work/ide/staging/_ide")
    set(_hw_ide "${_ide_dir}/${_xsa_base}")

    # Extract the bitstream / PDI and PS init script from the XSA (a zip),
    # mirroring the legacy vitis_launch_config recipe. Best-effort: a given XSA
    # contains only the members relevant to its architecture.
    include(${NO_OS_DIR}/cmake/xilinx/xilinx_hw.cmake)
    extract_xsa_members("${HARDWARE}" "${_hw_ide}")

    # A copy of the XSA lives in xsa_work (staged by config_xilinx_sdk); the FSBL
    # is produced by the flash target under xsa_run. Express both relative to the
    # workspace root so the generated config is portable.
    file(RELATIVE_PATH _xsa_rel "${_ws}"
        "${CMAKE_CURRENT_BINARY_DIR}/xsa_work/${_xsa_file}")
    file(RELATIVE_PATH _fsbl_rel "${_ws}"
        "${CMAKE_CURRENT_BINARY_DIR}/xsa_run/tmp/output/hw0/export/hw0/sw/hw0/boot/fsbl.elf")
    # The ELF the launch config downloads (built as <t>.elf under build/).
    set(_elf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_TARGET}.elf")

    execute_process(
        COMMAND ${Python3_EXECUTABLE}
                "${NO_OS_DIR}/tools/scripts/platform/xilinx/generate_vitis_launch.py"
                --arch "${XILINX_ARCH}"
                --project-name "${PROJECT_TARGET}"
                --xsa-path "${_xsa_rel}"
                --elf-path "${_elf}"
                --fsbl-path "${_fsbl_rel}"
                --project-dir "${_ws}"
                --output "${_ide_dir}/launch.json"
        RESULT_VARIABLE _rc OUTPUT_QUIET)
    if(NOT _rc EQUAL 0)
        message(STATUS "Vitis launch.json generation failed (rc=${_rc}); "
                       "the IDE can still be opened, debug config must be set up manually")
        return()
    endif()

    message(STATUS "Generated Vitis IDE config: ${_ide_dir}/launch.json")
    message(STATUS "Open the Vitis IDE with: no_os_build.py build ... --open")
endfunction()

function(ide_vitis_post_build PROJECT_TARGET)
    if(NOT PLATFORM STREQUAL "xilinx")
        return()
    endif()
    # launch.json paths are static; nothing to resolve post-build.
endfunction()
