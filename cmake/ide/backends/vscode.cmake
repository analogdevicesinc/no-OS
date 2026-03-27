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
    if(PLATFORM STREQUAL "maxim")
        set(OPENOCD_CONFIG "interface/cmsis-dap.cfg\",\n                \"target/${TARGET}.cfg")
    else()
        set(OPENOCD_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/openocd.cfg")
    endif()

    if(DEFINED OPENOCD_SCRIPTS)
        set(OPENOCD_SEARCH "${OPENOCD_SCRIPTS}")
    else()
        if(WIN32)
            set(OPENOCD_SEARCH "C:/openocd/share/openocd/scripts")
        elseif(APPLE)
            set(OPENOCD_SEARCH "/opt/homebrew/share/openocd/scripts;/usr/local/share/openocd/scripts")
        else()
            set(OPENOCD_SEARCH "/usr/share/openocd/scripts;/usr/local/share/openocd/scripts")
        endif()
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

    message(STATUS "Generated VS Code configuration in ${VSCODE_DIR}")
endfunction()

# No post-build steps needed — file(GENERATE) handles ELF path resolution.
function(ide_vscode_post_build PROJECT_TARGET)
endfunction()
