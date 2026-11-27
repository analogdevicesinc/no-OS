# Report a failed Vitis helper invocation (vitis -s util.py / xsct util.tcl).
#
# The Vitis tools are linked against host libraries that the Vitis installer
# does not ship; they come from the OS packages listed in
# <Vitis>/scripts/installLibs.sh. When one is missing, the dynamic loader error
# is buried in a Python traceback and CMake then piles unrelated compiler
# errors on top of it. Recognize that case and say which library is missing
# and how to get it; otherwise pass the tool's own stderr through.
#
#   WHAT    short description of the step that failed
#   RC      exit code of the helper
#   STDERR  captured stderr of the helper

function(xilinx_vitis_fatal WHAT RC STDERR)
    # The library name is matched by its own characters, not by the delimiters
    # around it: Python colors tracebacks with escape sequences on a terminal.
    if("${STDERR}" MATCHES "([A-Za-z0-9_+.-]+\\.so[0-9.]*): cannot open shared object file")
        message(FATAL_ERROR
            "${WHAT} failed: Vitis needs the host library ${CMAKE_MATCH_1}, "
            "which is not installed.\n"
            "Install the Vitis OS prerequisites:\n"
            "  sudo $ENV{XILINX_VITIS}/scripts/installLibs.sh\n"
            "If your Linux distribution is not handled by that script or does "
            "not package the library, see \"Host libraries\" in "
            "doc/sphinx/source/build_guides/build_xilinx_cmake.rst.")
    endif()
    message(FATAL_ERROR "${WHAT} failed (rc=${RC}).\n${STDERR}")
endfunction()
