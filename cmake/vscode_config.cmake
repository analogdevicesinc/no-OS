# --- VS Code Project Generation ---

# Ensure the .vscode directory exists in the source tree
set(VSCODE_DIR "${PROJECT_HOME}/.vscode")
file(MAKE_DIRECTORY ${VSCODE_DIR})

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