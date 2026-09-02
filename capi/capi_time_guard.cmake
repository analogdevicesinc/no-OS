# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 Analog Devices, Inc.
#
# Post-link guard for the capi_time time/delay backend.
#
# Run as a POST_BUILD step on a final executable:
#   cmake -DELF=<file> -DNM=<nm> -DPLATFORM=<plat> -P capi_time_guard.cmake
#
# It fails the build if a capi_time hook that has no usable weak default is
# present in the linked executable as a WEAK symbol. Because the whole capi
# time suite is compiled with -ffunction-sections and linked with
# --gc-sections, a capi_*_impl symbol survives into the ELF only when it is
# actually referenced (i.e. some code calls capi_wait_us/capi_wait_ms/
# capi_uptime). If such a reference resolves to the weak default from
# capi/src/capi_time.c instead of a strong platform backend, delays become
# no-ops and capi_uptime returns -ENOSYS at runtime. This turns that silent
# runtime failure into a loud build failure.
#
# Note: capi_wait_ms_impl is intentionally NOT guarded. Its weak default
# legitimately delegates to capi_wait_us_impl, so a platform that provides only
# the microsecond backend is still correct -- guarding capi_wait_ms_impl would
# wrongly fail such a platform.
#
# Symbols that are never used are removed by the linker and never reach this
# check, so projects that do not use capi_time are unaffected.
# This is a temporary guard. Eventually all platform will have their drivers
# ported to CAPI. This will become redundant.

if(NOT EXISTS "${ELF}")
    message(FATAL_ERROR "capi_time guard: executable not found: ${ELF}")
endif()

execute_process(
    COMMAND "${NM}" "${ELF}"
    OUTPUT_VARIABLE _nm_out
    RESULT_VARIABLE _nm_rc
    ERROR_VARIABLE  _nm_err
)
if(NOT _nm_rc EQUAL 0)
    message(FATAL_ERROR "capi_time guard: '${NM} ${ELF}' failed: ${_nm_err}")
endif()

set(_impls capi_wait_us_impl capi_uptime_impl)
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
        "capi_time: the following time hooks resolved to the WEAK "
        "no-op default in\n  ${ELF}:\n    ${_weak_str}\n\n"
        "capi_wait_us/capi_wait_ms/capi_uptime are used, but no strong platform "
        "backend was linked for PLATFORM=${PLATFORM}, so delays would be no-ops "
        "and capi_uptime would return -ENOSYS at runtime.\n"
        "Provide capi/platform/${PLATFORM}/${PLATFORM}_capi_time.c (defining "
        "capi_wait_us_impl and capi_uptime_impl, and optionally "
        "capi_wait_ms_impl), or do not use capi_time on this platform yet.")
endif()
