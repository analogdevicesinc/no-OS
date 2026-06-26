# Regenerate the per-build VS Code workspace from its template.
#
# Run both at configure time and as an always-out-of-date build step, so that
# `cmake --build` refreshes the workspace even when nothing else needs building
# (e.g. after the project sources or toolchain paths changed but the firmware
# is already up to date).
#
# Required -D variables: TEMPLATE, OUTPUT, NO_OS_PROJECT_NAME, CMAKE_SOURCE_DIR,
# CMAKE_BINARY_DIR, BUILD_DIR_NAME, GDB_PATH, OPENOCD_PATH.

configure_file("${TEMPLATE}" "${OUTPUT}" @ONLY)
