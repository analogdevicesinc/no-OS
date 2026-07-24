# Xilinx hardware-description (.xsa) helpers, shared across the Xilinx CMake flow.
#
# An .xsa is a zip archive. The bitstream and PS init scripts needed for JTAG
# programming and IDE debug live inside it:
#   *.bit          - PL bitstream (Zynq-7000 / ZynqMP / MicroBlaze)
#   *.pdi          - programmable device image (Versal)
#   ps7_init.tcl   - PS init sequence (Zynq-7000)
#   psu_init.tcl   - PS init sequence (ZynqMP / Cortex-R5)
#
# extract_xsa_members() unpacks whichever of these a given .xsa contains into a
# destination dir, using CMake's native archive support (file(ARCHIVE_EXTRACT),
# libarchive, available since 3.18) so the build does not depend on the external
# `unzip` binary.
#
# file(ARCHIVE_EXTRACT ... PATTERNS ...) is NOT best-effort: libarchive aborts
# fatally when a listed pattern matches no member, and a given .xsa only ships
# the members relevant to its architecture. So extract the whole archive into a
# scratch dir, then copy just the members we want when present.

function(extract_xsa_members XSA DEST_DIR)
    if(NOT EXISTS "${XSA}")
        message(WARNING "extract_xsa_members: .xsa not found: ${XSA}")
        return()
    endif()
    file(MAKE_DIRECTORY "${DEST_DIR}")

    set(_scratch "${DEST_DIR}/.xsa_extract")
    file(REMOVE_RECURSE "${_scratch}")
    file(MAKE_DIRECTORY "${_scratch}")
    file(ARCHIVE_EXTRACT INPUT "${XSA}" DESTINATION "${_scratch}")

    file(GLOB _members
        "${_scratch}/*.bit"
        "${_scratch}/*.pdi"
        "${_scratch}/ps7_init.tcl"
        "${_scratch}/psu_init.tcl")
    foreach(_m ${_members})
        file(COPY "${_m}" DESTINATION "${DEST_DIR}")
    endforeach()

    file(REMOVE_RECURSE "${_scratch}")
endfunction()
