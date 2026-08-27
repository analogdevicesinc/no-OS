# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 Analog Devices, Inc.
#
# Post-link guard for the capi_alloc allocation backend.
#
# Run as a POST_BUILD step on a final executable:
#   cmake -DELF=<file> -DNM=<nm> -DPLATFORM=<plat> -P capi_alloc_guard.cmake
#
# It fails the build if any capi_*_impl allocation hook is present in the
# linked executable as a WEAK symbol. Because the whole capi allocation suite
# is compiled with -ffunction-sections and linked with --gc-sections, a
# capi_*_impl symbol survives into the ELF only when it is actually referenced
# (i.e. some code calls capi_malloc/calloc/free/realloc). If such a reference
# resolves to the weak no-op default from capi/src/capi_alloc.c instead of a
# strong platform backend, every allocation would silently return NULL at
# runtime. This turns that silent runtime failure into a loud build failure.
#
# Symbols that are never used are removed by the linker and never reach this
# check, so projects that do not use capi_alloc are unaffected.
# This is a temporary guard. Eventually all platform will have their drivers 
# ported to CAPI. This will become redundant.

if(NOT EXISTS "${ELF}")
    message(FATAL_ERROR "capi_alloc guard: executable not found: ${ELF}")
endif()

execute_process(
    COMMAND "${NM}" "${ELF}"
    OUTPUT_VARIABLE _nm_out
    RESULT_VARIABLE _nm_rc
    ERROR_VARIABLE  _nm_err
)
if(NOT _nm_rc EQUAL 0)
    message(FATAL_ERROR "capi_alloc guard: '${NM} ${ELF}' failed: ${_nm_err}")
endif()

set(_impls capi_malloc_impl capi_calloc_impl capi_free_impl capi_realloc_impl)
set(_weak "")

# nm line for a defined symbol: "<hexvalue> <type> <name>"
# nm line for an undefined symbol: "<spaces> <type> <name>"
# Weak types: V/W (defined weak), v/w (undefined weak). Any of these for a
# capi_*_impl means no strong backend won the link.
string(REPLACE "\n" ";" _lines "${_nm_out}")
foreach(_line IN LISTS _lines)
    if(_line MATCHES "^[0-9a-fA-F]* +([VWvw]) +([A-Za-z0-9_]+)$")
        set(_type "${CMAKE_MATCH_1}")
        set(_name "${CMAKE_MATCH_2}")
        list(FIND _impls "${_name}" _idx)
        if(NOT _idx EQUAL -1)
            list(APPEND _weak "${_name} (weak '${_type}')")
        endif()
    endif()
endforeach()

if(_weak)
    string(REPLACE ";" "\n    " _weak_str "${_weak}")
    message(FATAL_ERROR
        "capi_alloc: the following allocation hooks resolved to the WEAK "
        "no-op default in\n  ${ELF}:\n    ${_weak_str}\n\n"
        "capi_malloc/calloc/free/realloc are used, but no strong platform "
        "backend was linked for PLATFORM=${PLATFORM}, so every allocation "
        "would return NULL at runtime.\n"
        "Provide capi/platform/${PLATFORM}/${PLATFORM}_capi_alloc.c (defining "
        "capi_malloc_impl, capi_calloc_impl, capi_free_impl and "
        "capi_realloc_impl), or do not use capi_alloc on this platform yet.")
endif()
