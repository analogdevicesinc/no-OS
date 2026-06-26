# --- VS Code IDE Backend ---
#
# Generates .vscode/ configuration files:
#   - launch.json      (debug configurations, needs ELF path via file(GENERATE))
#   - settings.json    (cortex-debug, cmake settings)
#   - c_cpp_properties.json (IntelliSense configuration)

set(VSCODE_TEMPLATES_DIR "${CMAKE_CURRENT_LIST_DIR}/../templates/vscode")

# Configure-time generation for settings.json and c_cpp_properties.json.
# These don't need generator expressions, so configure_file() is sufficient.
function(ide_vscode_configure PROJECT_TARGET)
    if(${NATIVE_BUILD})
        set(PROJECT_HOME "${CMAKE_SOURCE_DIR}")
    else()
        set(PROJECT_HOME "${CMAKE_SOURCE_DIR}")
    endif()

    set(VSCODE_DIR "${PROJECT_HOME}/.vscode")
    file(MAKE_DIRECTORY "${VSCODE_DIR}")

    # --- OpenOCD configuration for launch.json ---
    # Use the generated openocd.cfg produced by generate_openocd_config()
    set(OPENOCD_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg")

    # OPENOCD_SCRIPTS is set by the platform toolchain
    if(DEFINED OPENOCD_SCRIPTS)
        set(OPENOCD_SEARCH "${OPENOCD_SCRIPTS}")
    else()
        set(OPENOCD_SEARCH "")
    endif()

    # Point cortex-debug at the resolved OpenOCD binary so debugging works
    # even when openocd is not on PATH (e.g. only bundled with CubeIDE or the
    # Maxim SDK). When OpenOCD is not found, leave the placeholder empty so the
    # generated launch.json stays valid JSON.
    if(OPENOCD_PATH)
        set(OPENOCD_PATH_CONFIG "\n            \"serverpath\": \"${OPENOCD_PATH}\",")
    else()
        set(OPENOCD_PATH_CONFIG "")
    endif()

    # --- IntelliSense mode detection ---
    get_filename_component(COMPILER_NAME "${CMAKE_C_COMPILER}" NAME_WE)
    if("${COMPILER_NAME}" MATCHES "clang")
        set(INTELLISENSE_MODE "clang-arm")
    elseif("${COMPILER_NAME}" MATCHES "gcc" OR "${COMPILER_NAME}" MATCHES "arm-none-eabi-gcc")
        set(INTELLISENSE_MODE "gcc-arm")
    else()
        set(INTELLISENSE_MODE "gcc-arm")
    endif()

    # --- Relative build directory for compile_commands.json ---
    file(RELATIVE_PATH CMAKE_BUILD_DIR "${PROJECT_HOME}" "${CMAKE_BINARY_DIR}")

    # Only expose the build directory as a workspace folder when it lives inside
    # the project home. For out-of-tree builds the relative path escapes the
    # repo (e.g. "../../tmp/build"), which VS Code cannot resolve and which
    # breaks the whole workspace, so omit the folder in that case.
    if(CMAKE_BUILD_DIR MATCHES "^\\.\\." OR IS_ABSOLUTE "${CMAKE_BUILD_DIR}")
        set(WORKSPACE_BUILD_FOLDER "")
    else()
        set(WORKSPACE_BUILD_FOLDER
"        {
            \"name\": \"${CMAKE_BUILD_DIR} [build directory]\",
            \"path\": \"${CMAKE_BUILD_DIR}\"
        },
")
    endif()

    # --- Platform SDK include paths for IntelliSense fallback ---
    # Files not in compile_commands.json use includePath as fallback.
    set(PLATFORM_INCLUDE_PATHS "")
    if(DEFINED MAXIM_LIBRARIES)
        set(PLATFORM_INCLUDE_PATHS ",\n                \"${MAXIM_LIBRARIES}/**\"")
    endif()

    # --- Generate settings.json and c_cpp_properties.json (no genex needed) ---
    configure_file(
        "${VSCODE_TEMPLATES_DIR}/settings.json.in"
        "${VSCODE_DIR}/settings.json"
        @ONLY
    )

    configure_file(
        "${VSCODE_TEMPLATES_DIR}/c_cpp_properties.json.in"
        "${VSCODE_DIR}/c_cpp_properties.json"
        @ONLY
    )

    # --- Generate launch.json via two-pass rendering ---
    # Pass 1: configure_file() resolves @VAR@ placeholders → intermediate file
    # Pass 2: file(GENERATE) resolves $<GENEX> → final output
    set(PROJECT_TARGET_NAME "${PROJECT_TARGET}")
    configure_file(
        "${VSCODE_TEMPLATES_DIR}/launch.json.in"
        "${CMAKE_BINARY_DIR}/_ide_tmp/launch.json.gen"
        @ONLY
    )
    file(GENERATE
        OUTPUT "${VSCODE_DIR}/launch.json"
        INPUT "${CMAKE_BINARY_DIR}/_ide_tmp/launch.json.gen"
        TARGET ${PROJECT_TARGET}
    )

    # --- Generate multi-root workspace file ---
    # Places the project directory as the first root in VS Code's explorer,
    # making project sources easy to find among the full no-OS tree.
    if(DEFINED NO_OS_PROJECT_NAME)
        configure_file(
            "${VSCODE_TEMPLATES_DIR}/no-os.code-workspace.in"
            "${PROJECT_HOME}/no-os.code-workspace"
            @ONLY
        )
        message(STATUS "Generated VS Code workspace: ${PROJECT_HOME}/no-os.code-workspace")
        message(STATUS "Open it with: code ${PROJECT_HOME}/no-os.code-workspace")
    endif()

    message(STATUS "Generated VS Code configuration in ${VSCODE_DIR}")
endfunction()

# No post-build steps needed — file(GENERATE) handles ELF path resolution.
function(ide_vscode_post_build PROJECT_TARGET)
endfunction()
