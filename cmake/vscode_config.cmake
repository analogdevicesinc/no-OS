# --- VS Code Project Generation ---

# Ensure the .vscode directory exists in the source tree
set(VSCODE_DIR "${PROJECT_HOME}/.vscode")
file(MAKE_DIRECTORY ${VSCODE_DIR})

# Set OpenOCD configuration based on platform
if("${PLATFORM}" STREQUAL "maxim")
    set(OPENOCD_CONFIG "\"interface/cmsis-dap.cfg\",\n                \"target/${TARGET}.cfg\"")
else()
    set(OPENOCD_CONFIG ${CMAKE_BINARY_DIR}/openocd.cfg)
endif()
# Set OpenOCD search directory based on platform
if(DEFINED OPENOCD_SCRIPTS)
    set(OPENOCD_SEARCH ${OPENOCD_SCRIPTS})
else()
    # Fallback to common OpenOCD installation paths
    if(WIN32)
        set(OPENOCD_SEARCH "C:/openocd/share/openocd/scripts")
    elseif(APPLE)
        set(OPENOCD_SEARCH "/opt/homebrew/share/openocd/scripts;/usr/local/share/openocd/scripts")
    else()
        set(OPENOCD_SEARCH "/usr/share/openocd/scripts;/usr/local/share/openocd/scripts")
    endif()
endif()

# Set OpenOCD path configuration for cortex-debug
if(DEFINED OPENOCD_PATH AND NOT "${OPENOCD_PATH}" STREQUAL "")
    set(OPENOCD_PATH_CONFIG "\n            \"openocdPath\": \"${OPENOCD_PATH}\",")
else()
    set(OPENOCD_PATH_CONFIG "")
endif()

file(REMOVE "${VSCODE_DIR}/launch.json")
file(REMOVE "${VSCODE_DIR}/settings.json")

# Generate the launch.json file from the template
configure_file(
  "${NO_OS_DIR}/cmake/templates/launch.json.in"
  "${VSCODE_DIR}/launch.json"
  @ONLY
)

configure_file(
  "${NO_OS_DIR}/cmake/templates/settings.json.in"
  "${VSCODE_DIR}/settings.json"
)

message(STATUS "Generated VS Code configuration in ${VSCODE_DIR}")