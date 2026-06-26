# Error-reporting helpers for the no-OS CMake build.
#
# These keep failure messages uniform and actionable: instead of letting a
# missing tool or a failed sub-process surface later as a cryptic error (a
# missing no_os_config.h, a generic compiler-detection failure, ...), the
# helpers stop the configure with a message that names the missing item and
# how to fix it.

# Abort with FATAL_ERROR if VAR_NAME is unset, empty, or a *-NOTFOUND value.
# Use for required variables and find_program/find_package results.
#   no_os_require(PLATFORM "PLATFORM is not set (e.g. -DPLATFORM=maxim).")
function(no_os_require VAR_NAME MESSAGE)
    if(NOT DEFINED ${VAR_NAME}
       OR "${${VAR_NAME}}" STREQUAL ""
       OR "${${VAR_NAME}}" MATCHES "-NOTFOUND$")
        message(FATAL_ERROR "${MESSAGE}")
    endif()
endfunction()

# Abort with FATAL_ERROR if PATH does not exist on disk.
# Use for required files/directories (toolchain file, defconfig, SDK root, ...).
#   no_os_require_path("${TOOLCHAIN}" "Toolchain file not found: ${TOOLCHAIN}")
function(no_os_require_path PATH MESSAGE)
    if(NOT EXISTS "${PATH}")
        message(FATAL_ERROR "${MESSAGE}")
    endif()
endfunction()

# Run a command via execute_process and FATAL_ERROR on a non-zero exit,
# echoing the captured output so the cause is visible.
#
#   no_os_run_checked(
#       WHAT "generating build config"
#       COMMAND ${PYTHON} ${SCRIPT} ...
#       [WORKING_DIRECTORY <dir>])
#
# On success the command's stdout/stderr are forwarded to the configure log so
# behaviour matches the previous unchecked execute_process calls.
function(no_os_run_checked)
    cmake_parse_arguments(ARG "" "WHAT;WORKING_DIRECTORY" "COMMAND" ${ARGN})

    if(NOT ARG_COMMAND)
        message(FATAL_ERROR "no_os_run_checked: COMMAND is required")
    endif()
    if(NOT ARG_WHAT)
        set(ARG_WHAT "running command")
    endif()

    set(_wd_arg "")
    if(ARG_WORKING_DIRECTORY)
        set(_wd_arg WORKING_DIRECTORY "${ARG_WORKING_DIRECTORY}")
    endif()

    execute_process(
        COMMAND ${ARG_COMMAND}
        ${_wd_arg}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _error
    )

    if(_output)
        message(STATUS "${_output}")
    endif()

    if(NOT _result EQUAL 0)
        if(_error)
            message(STATUS "${_error}")
        endif()
        message(FATAL_ERROR
            "Failed while ${ARG_WHAT} (exit code ${_result}).\n"
            "Command: ${ARG_COMMAND}\n"
            "See the output above for details.")
    elseif(_error)
        # Non-fatal: tools often write progress to stderr.
        message(STATUS "${_error}")
    endif()
endfunction()
