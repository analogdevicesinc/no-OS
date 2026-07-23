# LibraryCacheUtils.cmake
# Resolves a dependency's source tree (see resolve_library_source).

find_package(Git QUIET)

# A 7+ char hex string is a commit SHA; it can't be cloned with --branch.
function(is_git_sha VERSION OUT_VAR)
    if("${VERSION}" MATCHES "^[0-9a-fA-F]+$" AND "${VERSION}" MATCHES "^.......")
        set(${OUT_VAR} TRUE PARENT_SCOPE)
    else()
        set(${OUT_VAR} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Shallow-clone a single ref (tag/branch/SHA) into DEST, fetching only that ref.
# Sets RESULT_VAR to 0 on success. A bare SHA can't be --branch'd, so init+fetch it.
function(no_os_git_clone URL REF DEST RESULT_VAR)
    find_package(Git REQUIRED)
    is_git_sha("${REF}" _is_sha)
    if(_is_sha)
        file(MAKE_DIRECTORY "${DEST}")
        execute_process(COMMAND ${GIT_EXECUTABLE} init -q "${DEST}" RESULT_VARIABLE _r)
        if(_r EQUAL 0)
            execute_process(COMMAND ${GIT_EXECUTABLE} -C "${DEST}" remote add origin "${URL}" RESULT_VARIABLE _r)
        endif()
        if(_r EQUAL 0)
            execute_process(COMMAND ${GIT_EXECUTABLE} -C "${DEST}" fetch -q --depth 1 origin "${REF}" RESULT_VARIABLE _r)
        endif()
        if(_r EQUAL 0)
            execute_process(COMMAND ${GIT_EXECUTABLE} -C "${DEST}" checkout -q FETCH_HEAD RESULT_VARIABLE _r)
        endif()
    else()
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone -q --depth 1 --single-branch --branch "${REF}"
                    --config advice.detachedHead=false "${URL}" "${DEST}"
            RESULT_VARIABLE _r)
    endif()
    set(${RESULT_VAR} ${_r} PARENT_SCOPE)
endfunction()

# TRUE if the git checkout at DIR already sits on the commit REF names.
function(no_os_checkout_matches DIR REF OUT_VAR)
    set(${OUT_VAR} FALSE PARENT_SCOPE)
    if(NOT GIT_FOUND OR NOT EXISTS "${DIR}/.git")
        return()
    endif()
    execute_process(COMMAND ${GIT_EXECUTABLE} -C "${DIR}" rev-parse HEAD
        OUTPUT_VARIABLE _head OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET RESULT_VARIABLE _hr)
    execute_process(COMMAND ${GIT_EXECUTABLE} -C "${DIR}" rev-parse "${REF}^{commit}"
        OUTPUT_VARIABLE _want OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET RESULT_VARIABLE _wr)
    if(_hr EQUAL 0 AND _wr EQUAL 0 AND "${_head}" STREQUAL "${_want}")
        set(${OUT_VAR} TRUE PARENT_SCOPE)
    endif()
endfunction()

# Resolve a dependency's source tree. Two tiers:
#
#   1. Override: NO_OS_<LIB>_DIR (CMake var or env) is used verbatim, no checks.
#      Lets a tool point one library at a specific working copy - e.g. building
#      several repos against each dependency's main to catch regressions.
#
#   2. Managed clone: reuse a clone if it already holds the requested commit,
#      otherwise clone that ref. Its location is:
#        - libraries/<repo> in-tree by default: cloned once, reused on every
#          later configure in the same tree (the common developer case).
#        - <NO_OS_CACHE_DIR>/<lib>/<version> when NO_OS_CACHE_DIR is set: a
#          shared, version-keyed store many builds reuse - e.g. a CI cache
#          persisted across pipeline runs so deps aren't recloned each build.
#
# Arguments:
#   LIB_NAME          - Library identifier (e.g., "freertos", "lwip")
#   REQUESTED_VERSION - Tag, branch, or commit SHA to resolve
#   IN_TREE_PATH      - Default clone path (relative to NO_OS_DIR or absolute)
#   GIT_REPO_URL      - Git repository URL to clone from
#   OUT_SOURCE_DIR    - Output variable for resolved source directory
#   OUT_BINARY_DIR    - Output variable for build directory
function(resolve_library_source LIB_NAME REQUESTED_VERSION IN_TREE_PATH GIT_REPO_URL OUT_SOURCE_DIR OUT_BINARY_DIR)

    message(STATUS "[${LIB_NAME}] Resolving version: ${REQUESTED_VERSION}")
    set(${OUT_BINARY_DIR} "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-build" PARENT_SCOPE)

    if(NOT IS_ABSOLUTE "${IN_TREE_PATH}")
        set(IN_TREE_PATH "${NO_OS_DIR}/${IN_TREE_PATH}")
    endif()

    # --- Tier 1: NO_OS_<LIB>_DIR override, used as-is ---
    string(TOUPPER "${LIB_NAME}" _lib_upper)
    set(_override_var "NO_OS_${_lib_upper}_DIR")
    set(_override_dir "")
    if(DEFINED ${_override_var} AND NOT "${${_override_var}}" STREQUAL "")
        set(_override_dir "${${_override_var}}")
    elseif(DEFINED ENV{${_override_var}} AND NOT "$ENV{${_override_var}}" STREQUAL "")
        set(_override_dir "$ENV{${_override_var}}")
    endif()
    if(NOT "${_override_dir}" STREQUAL "")
        if(NOT IS_ABSOLUTE "${_override_dir}")
            set(_override_dir "${CMAKE_SOURCE_DIR}/${_override_dir}")
        endif()
        message(STATUS "[${LIB_NAME}] ✓ Using ${_override_var} override: ${_override_dir}")
        set(${OUT_SOURCE_DIR} "${_override_dir}" PARENT_SCOPE)
        return()
    endif()

    # --- Tier 2: managed clone (shared cache slot, or in-tree by default) ---
    if(DEFINED ENV{NO_OS_CACHE_DIR} AND NOT "$ENV{NO_OS_CACHE_DIR}" STREQUAL "")
        set(_dest "$ENV{NO_OS_CACHE_DIR}/${LIB_NAME}/${REQUESTED_VERSION}")
    else()
        set(_dest "${IN_TREE_PATH}")
    endif()

    # Reuse the clone if it already sits on the requested commit.
    no_os_checkout_matches("${_dest}" "${REQUESTED_VERSION}" _matches)
    if(_matches)
        message(STATUS "[${LIB_NAME}] ✓ Reusing clone at ${_dest}")
        set(${OUT_SOURCE_DIR} "${_dest}" PARENT_SCOPE)
        return()
    endif()

    # A leftover dir (wrong version, or an interrupted clone) would make the
    # clone fail on a non-empty target; remove it for a clean checkout.
    if(EXISTS "${_dest}")
        file(REMOVE_RECURSE "${_dest}")
    endif()
    get_filename_component(_dest_parent "${_dest}" DIRECTORY)
    file(MAKE_DIRECTORY "${_dest_parent}")

    message(STATUS "[${LIB_NAME}] → Cloning ${REQUESTED_VERSION} to: ${_dest}")
    no_os_git_clone("${GIT_REPO_URL}" "${REQUESTED_VERSION}" "${_dest}" _clone_res)
    if(NOT _clone_res EQUAL 0)
        message(FATAL_ERROR "[${LIB_NAME}] Failed to clone ${REQUESTED_VERSION} from ${GIT_REPO_URL}")
    endif()

    set(${OUT_SOURCE_DIR} "${_dest}" PARENT_SCOPE)
endfunction()
