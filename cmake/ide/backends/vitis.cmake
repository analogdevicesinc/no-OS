# --- Vitis IDE Backend (Xilinx) ---
#
# Minimal backend for the CMake-driven Xilinx flow. The heavy lifting (BSP +
# linker script generation) happens in config_xilinx_sdk()
# (cmake/xilinx/xilinx_platform_sdk.cmake), not here. This backend is a
# placeholder for future Vitis launch.json / debug-config generation via
# tools/scripts/platform/xilinx/generate_vitis_launch.py (which needs the built
# ELF + FSBL, so it belongs in a post-build step once boot/flash is wired up).

function(ide_vitis_configure PROJECT_TARGET)
    if(NOT PLATFORM STREQUAL "xilinx")
        return()
    endif()
    # No configure-time IDE artifacts yet.
endfunction()

function(ide_vitis_post_build PROJECT_TARGET)
    if(NOT PLATFORM STREQUAL "xilinx")
        return()
    endif()
    # No post-build IDE artifacts yet.
endfunction()
