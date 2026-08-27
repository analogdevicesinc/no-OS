# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 Analog Devices, Inc.
#
# Generic wiring for capi_alloc
# Will become generic wiring for entire CAPI

# 1. Header path (always on -- harmless where capi is unused).
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# 2. Generic wrapper (always compiled).
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_alloc.c)

# 3. Platform backend, only if this PLATFORM provides one.
set(_capi_backend
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${PLATFORM}/${PLATFORM}_capi_alloc.c)
if(EXISTS "${_capi_backend}")
    target_sources(no-os PRIVATE ${_capi_backend})
    message(STATUS "capi_alloc: using ${PLATFORM} backend")
else()
    message(STATUS "capi_alloc: no ${PLATFORM} backend -- weak no-op defaults "
                   "in effect (a link guard fails any project that uses capi_alloc)")
endif()

# Resolve an 'nm' matching the toolchain for the post-link guard, and stash the
# results in cache/global scope so capi_alloc_add_link_guard() -- which is
# called from another directory (cmake/project_utils.cmake) -- can see them.
if(CMAKE_NM)
    set(_capi_nm "${CMAKE_NM}")
else()
    get_filename_component(_cc "${CMAKE_C_COMPILER}" NAME)
    get_filename_component(_cc_dir "${CMAKE_C_COMPILER}" DIRECTORY)
    string(REGEX REPLACE "(gcc|cc|clang)(\\.exe)?$" "nm" _nm_name "${_cc}")
    find_program(_capi_nm NAMES "${_nm_name}" nm HINTS "${_cc_dir}")
endif()
set(CAPI_ALLOC_NM "${_capi_nm}" CACHE INTERNAL "nm used by the capi_alloc link guard")
set(CAPI_ALLOC_GUARD_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/capi_alloc_guard.cmake"
    CACHE INTERNAL "path to the capi_alloc post-link guard script")

# Attach the post-link weak-symbol guard to an executable target. Safe to call
# on every project: it passes silently unless capi_*_impl is referenced but
# resolves to the weak no-op default (no strong backend linked).
function(capi_alloc_add_link_guard _target)
    if(NOT CAPI_ALLOC_NM)
        message(WARNING "capi_alloc: no 'nm' found; skipping weak-symbol link "
                        "guard for ${_target} (allocation backend not verified)")
        return()
    endif()
    add_custom_command(
        TARGET ${_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
                -DELF=$<TARGET_FILE:${_target}>
                -DNM=${CAPI_ALLOC_NM}
                -DPLATFORM=${PLATFORM}
                -P ${CAPI_ALLOC_GUARD_SCRIPT}
        VERBATIM
        COMMENT "capi_alloc: verifying strong allocation backend for ${_target}")
endfunction()
