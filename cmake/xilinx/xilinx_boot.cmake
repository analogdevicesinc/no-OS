# Boot-image description for the Xilinx CMake flow.
#
# Everything needed to package an SD-card BOOT.BIN is already known to CMake at
# configure time: the arch came out of the .xsa in the toolchain file, the
# bitstream was unzipped by extract_xsa_members(), and the ELF and FSBL paths
# are fixed by the build layout. write_xilinx_boot_info() writes those facts to
# <build>/bootinfo.json so the packager
# (tools/scripts/platform/xilinx/boot_bin.py) does not have to rediscover any of
# them -- no CMakeCache.txt grepping, no second unzip of the .xsa.
#
# Paths are absolute and describe where each file will be, not where it is: the
# ELF appears at build time and the FSBL only when the packager or the 'flash'
# target generates it. The packager checks for itself.
#
# Schema (version 1):
#   version    - integer, bumped on incompatible changes
#   project    - CMake target name; the ELF and .bif are named after it
#   arch       - HSI cell name from the .xsa (psu_cortexa53_0, ps7_cortexa9_0,
#                sys_mb, ...). The packager maps this to bootgen's -arch and the
#                BIF destination_cpu.
#   xsa        - the hardware file this build was configured against
#   elf        - the application ELF
#   bitstream  - PL bitstream extracted from the .xsa ("" if it ships none)
#   fsbl       - where create_fsbl puts the boot loader
#   fsbl_ws    - workspace/hw_path pair util.py create_fsbl expects (ws, ws/tmp)
#   output_dir - where the packager should leave BOOT.BIN

function(write_xilinx_boot_info TARGET_NAME RUN_DIR HW_PATH ELF FSBL)
    # create_fsbl reads the arch from <hw_path>/arch.txt and the .xsa from
    # <hw_path>. The 'flash' target stages both when it runs; stage them here
    # too so the packager can generate an FSBL without having flashed first.
    # (The flash target's copy_if_different then becomes a no-op.)
    file(WRITE "${HW_PATH}/arch.txt" "${XILINX_ARCH}\n")
    file(COPY "${HARDWARE}" DESTINATION "${HW_PATH}")

    # extract_xsa_members() has already unpacked these next to the .xsa copy.
    file(GLOB _bit "${HW_PATH}/*.bit")
    if(NOT _bit)
        file(GLOB _bit "${HW_PATH}/*.pdi")
    endif()
    list(GET _bit 0 _bitstream)
    if(NOT _bitstream)
        set(_bitstream "")
    endif()

    # A backslash is an escape inside a JSON string, and -DHARDWARE=C:\... on
    # Windows reaches us unnormalized. CMake itself accepts forward slashes on
    # every host, and so do bootgen and the Vitis Python API.
    foreach(_var HARDWARE ELF FSBL RUN_DIR _bitstream)
        string(REPLACE "\\" "/" ${_var} "${${_var}}")
    endforeach()

    set(_info "${CMAKE_BINARY_DIR}/bootinfo.json")
    string(JOIN "\n" _json
        "{"
        "  \"version\": 1,"
        "  \"project\": \"${TARGET_NAME}\","
        "  \"arch\": \"${XILINX_ARCH}\","
        "  \"xsa\": \"${HARDWARE}\","
        "  \"elf\": \"${ELF}\","
        "  \"bitstream\": \"${_bitstream}\","
        "  \"fsbl\": \"${FSBL}\","
        "  \"fsbl_ws\": \"${RUN_DIR}\","
        "  \"output_dir\": \"${CMAKE_BINARY_DIR}/boot\""
        "}")
    file(WRITE "${_info}" "${_json}\n")
endfunction()
