# Pre-project() compiler resolution for the Altera / Nios V platform.
#
# CMAKE_C/CXX/ASM_COMPILER must each be a full path before project() triggers
# compiler detection (a bare name on PATH is not supported for this
# toolchain), so this runs from root CMakeLists.txt ahead of project().

# Resolve and validate CMAKE_<LANG>_COMPILER (LANG = C, CXX or ASM): prefer
# the environment variable, fall back to an existing cache/CLI value, require
# a full path, and force the result into cache.
function(_altera_require_compiler LANG)
    set(_compiler "")
    if(DEFINED ENV{CMAKE_${LANG}_COMPILER} AND NOT "$ENV{CMAKE_${LANG}_COMPILER}" STREQUAL "")
        set(_compiler "$ENV{CMAKE_${LANG}_COMPILER}")
    elseif(DEFINED CMAKE_${LANG}_COMPILER AND NOT CMAKE_${LANG}_COMPILER STREQUAL "")
        if(IS_ABSOLUTE "${CMAKE_${LANG}_COMPILER}" OR "${CMAKE_${LANG}_COMPILER}" MATCHES "/")
            set(_compiler "${CMAKE_${LANG}_COMPILER}")
        else()
            message(FATAL_ERROR "Altera toolchain requires a full path in CMAKE_${LANG}_COMPILER; got '${CMAKE_${LANG}_COMPILER}'.")
        endif()
    endif()

    if(_compiler STREQUAL "")
        message(FATAL_ERROR "Altera toolchain requires -DCMAKE_${LANG}_COMPILER=<full-path> or export CMAKE_${LANG}_COMPILER=<full-path> before configuring.")
    endif()
    set(CMAKE_${LANG}_COMPILER "${_compiler}" CACHE FILEPATH "${LANG} compiler" FORCE)
endfunction()
