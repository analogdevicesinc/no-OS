# LibraryCacheUtils.cmake
# Common utilities for library caching with hybrid priority:
# 1. Submodule (if matches requested version)
# 2. Global cache (if version exists)
# 3. FetchContent (download to cache or build dir)

include(FetchContent)

# Find git for version checking
find_package(Git QUIET)

# Download-only populate that works across CMake versions.
#
# The single-argument FetchContent_Populate() is intentional here: we want to
# clone the sources WITHOUT add_subdirectory()'ing the library's own build
# system. CMake 3.30 deprecated that form (policy CMP0169) and makes it a hard
# error under NEW behavior, so set the policy to OLD locally for the call. The
# guard keeps it working on the declared minimum (3.27, where CMP0169 does not
# exist) as well as on 3.30+.
#
# Implemented as a macro (not a function) so that the <lcname>_SOURCE_DIR /
# <lcname>_BINARY_DIR variables FetchContent_Populate() defines land in the
# caller's scope, where the build-dir fallback below reads them.
macro(no_os_fetchcontent_populate LIB_NAME)
    if(POLICY CMP0169)
        cmake_policy(PUSH)
        cmake_policy(SET CMP0169 OLD)
    endif()
    FetchContent_Populate(${LIB_NAME})
    if(POLICY CMP0169)
        cmake_policy(POP)
    endif()
endmacro()

# Determine cache directory from environment or use default
function(get_no_os_cache_dir OUT_VAR)
    if(DEFINED ENV{NO_OS_CACHE_DIR})
        set(${OUT_VAR} "$ENV{NO_OS_CACHE_DIR}" PARENT_SCOPE)
    else()
        # Default to .cache/ in the no-OS root directory
        set(${OUT_VAR} "${NO_OS_DIR}/.cache" PARENT_SCOPE)
    endif()
endfunction()

# Get version tag from a git repository directory
# Returns empty string if not a git repo or on error
function(get_git_version REPO_DIR OUT_VAR)
    if(NOT GIT_FOUND)
        set(${OUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    if(NOT EXISTS "${REPO_DIR}/.git" AND NOT EXISTS "${REPO_DIR}/HEAD")
        set(${OUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    # Try to get exact tag match first
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --exact-match HEAD
        WORKING_DIRECTORY "${REPO_DIR}"
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE GIT_RESULT
    )

    if(GIT_RESULT EQUAL 0 AND GIT_TAG)
        set(${OUT_VAR} "${GIT_TAG}" PARENT_SCOPE)
        return()
    endif()

    # Fallback to describe with distance (for development versions)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags HEAD
        WORKING_DIRECTORY "${REPO_DIR}"
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE GIT_RESULT
    )

    if(GIT_RESULT EQUAL 0 AND GIT_TAG)
        set(${OUT_VAR} "${GIT_TAG}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# Check if submodule version matches requested version
# Handles both exact matches and version prefixes
function(check_version_match ACTUAL_VERSION REQUESTED_VERSION OUT_VAR)
    if("${ACTUAL_VERSION}" STREQUAL "${REQUESTED_VERSION}")
        set(${OUT_VAR} TRUE PARENT_SCOPE)
    elseif("${ACTUAL_VERSION}" MATCHES "^${REQUESTED_VERSION}")
        # Matches if actual starts with requested (e.g., "V10.4.3-707-..." matches "V10.4.3")
        set(${OUT_VAR} TRUE PARENT_SCOPE)
    else()
        set(${OUT_VAR} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Main function to resolve library source with hybrid priority
# Arguments:
#   LIB_NAME         - Library identifier (e.g., "freertos", "lwip")
#   REQUESTED_VERSION - Version tag to use
#   SUBMODULE_PATH   - Path to submodule directory (relative to NO_OS_DIR or absolute)
#   GIT_REPO_URL     - Git repository URL for FetchContent
#   OUT_SOURCE_DIR   - Output variable for resolved source directory
#   OUT_BINARY_DIR   - Output variable for build directory
function(resolve_library_source LIB_NAME REQUESTED_VERSION SUBMODULE_PATH GIT_REPO_URL OUT_SOURCE_DIR OUT_BINARY_DIR)

    message(STATUS "[${LIB_NAME}] Resolving version: ${REQUESTED_VERSION}")

    # Make submodule path absolute if needed
    if(NOT IS_ABSOLUTE "${SUBMODULE_PATH}")
        set(SUBMODULE_PATH "${NO_OS_DIR}/${SUBMODULE_PATH}")
    endif()

    # --- Priority 1: Check submodule ---
    if(EXISTS "${SUBMODULE_PATH}/.git" OR EXISTS "${SUBMODULE_PATH}/CMakeLists.txt" OR EXISTS "${SUBMODULE_PATH}/src")
        get_git_version("${SUBMODULE_PATH}" SUBMODULE_VERSION)

        if(SUBMODULE_VERSION)
            check_version_match("${SUBMODULE_VERSION}" "${REQUESTED_VERSION}" VERSION_MATCHES)

            if(VERSION_MATCHES)
                message(STATUS "[${LIB_NAME}] ✓ Using submodule (version: ${SUBMODULE_VERSION})")
                set(${OUT_SOURCE_DIR} "${SUBMODULE_PATH}" PARENT_SCOPE)
                set(${OUT_BINARY_DIR} "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-build" PARENT_SCOPE)
                return()
            else()
                message(STATUS "[${LIB_NAME}] Submodule version (${SUBMODULE_VERSION}) != requested (${REQUESTED_VERSION})")
            endif()
        else()
            # Submodule exists but no version info - assume it might match
            message(STATUS "[${LIB_NAME}] ✓ Using submodule (version unknown, assuming compatible)")
            set(${OUT_SOURCE_DIR} "${SUBMODULE_PATH}" PARENT_SCOPE)
            set(${OUT_BINARY_DIR} "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-build" PARENT_SCOPE)
            return()
        endif()
    else()
        message(STATUS "[${LIB_NAME}] Submodule not found at ${SUBMODULE_PATH}")
    endif()

    # --- Priority 2: Check global cache ---
    get_no_os_cache_dir(CACHE_DIR)
    set(CACHED_LIB_DIR "${CACHE_DIR}/${LIB_NAME}/${REQUESTED_VERSION}")

    if(EXISTS "${CACHED_LIB_DIR}/CMakeLists.txt" OR EXISTS "${CACHED_LIB_DIR}/src")
        message(STATUS "[${LIB_NAME}] ✓ Using cached version at ${CACHED_LIB_DIR}")
        set(${OUT_SOURCE_DIR} "${CACHED_LIB_DIR}" PARENT_SCOPE)
        set(${OUT_BINARY_DIR} "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-build" PARENT_SCOPE)
        return()
    else()
        message(STATUS "[${LIB_NAME}] Version ${REQUESTED_VERSION} not in cache (${CACHED_LIB_DIR})")
    endif()

    # --- Priority 3: FetchContent ---
    # Decide where to fetch: to cache (if cache dir is writable) or to build dir
    set(FETCH_TO_CACHE FALSE)
    if(NOT "${CACHE_DIR}" STREQUAL "")
        # Check if we can write to cache directory
        get_filename_component(CACHE_PARENT "${CACHE_DIR}" DIRECTORY)
        if(EXISTS "${CACHE_DIR}" OR EXISTS "${CACHE_PARENT}")
            set(FETCH_TO_CACHE TRUE)
        endif()
    endif()

    if(FETCH_TO_CACHE)
        message(STATUS "[${LIB_NAME}] → Fetching ${REQUESTED_VERSION} to cache: ${CACHED_LIB_DIR}")

        # FetchContent picks clone-vs-update from the subbuild stamps, so a
        # missing/corrupt cache checkout with stale stamps makes it "update" a
        # non-repo and fail. Scrub the cache dir and the stamps (under the
        # top-level _deps, not CMAKE_CURRENT_BINARY_DIR) so a clean clone runs.
        string(TOLOWER "${LIB_NAME}" LIB_NAME_LOWER)
        if(DEFINED FETCHCONTENT_BASE_DIR AND NOT "${FETCHCONTENT_BASE_DIR}" STREQUAL "")
            set(_fc_base "${FETCHCONTENT_BASE_DIR}")
        else()
            set(_fc_base "${CMAKE_BINARY_DIR}/_deps")
        endif()
        if(NOT EXISTS "${CACHED_LIB_DIR}/.git")
            message(STATUS "[${LIB_NAME}] Cache checkout missing or not a git repo; scrubbing stale subbuild stamps for a clean clone")
            file(REMOVE_RECURSE "${CACHED_LIB_DIR}")
            file(REMOVE_RECURSE "${_fc_base}/${LIB_NAME_LOWER}-subbuild")
        endif()

        # Create cache directory structure
        file(MAKE_DIRECTORY "${CACHE_DIR}/${LIB_NAME}")

        # Use FetchContent_Populate (not MakeAvailable) to download only,
        # without calling add_subdirectory on the library's CMakeLists.txt.
        # This avoids processing third-party build systems that may create
        # unwanted targets or fail for cross-compilation.
        FetchContent_Declare(
            ${LIB_NAME}
            GIT_REPOSITORY ${GIT_REPO_URL}
            GIT_TAG ${REQUESTED_VERSION}
            GIT_SHALLOW TRUE
            SOURCE_DIR "${CACHED_LIB_DIR}"
        )

        no_os_fetchcontent_populate(${LIB_NAME})

        set(${OUT_SOURCE_DIR} "${CACHED_LIB_DIR}" PARENT_SCOPE)
        set(${OUT_BINARY_DIR} "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-build" PARENT_SCOPE)
    else()
        message(STATUS "[${LIB_NAME}] → Fetching ${REQUESTED_VERSION} to build directory (cache not writable)")

        FetchContent_Declare(
            ${LIB_NAME}
            GIT_REPOSITORY ${GIT_REPO_URL}
            GIT_TAG ${REQUESTED_VERSION}
            GIT_SHALLOW TRUE
        )

        no_os_fetchcontent_populate(${LIB_NAME})

        # Get the actual source/binary dirs from FetchContent
        string(TOLOWER "${LIB_NAME}" LIB_NAME_LOWER)
        set(${OUT_SOURCE_DIR} "${${LIB_NAME_LOWER}_SOURCE_DIR}" PARENT_SCOPE)
        set(${OUT_BINARY_DIR} "${${LIB_NAME_LOWER}_BINARY_DIR}" PARENT_SCOPE)
    endif()
endfunction()
