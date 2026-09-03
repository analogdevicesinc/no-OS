# Glob versioned install dirs — Linux: /opt/st/stm32cubeide_*/ and ~/st/stm32cubeide_*/
# Windows: C:/ST/STM32CubeIDE_*/  macOS: /Applications/STM32CubeIDE_*/
file(GLOB _cubeide_hints
    "/opt/st/stm32cubeide_*"
    "$ENV{HOME}/st/stm32cubeide_*"
    "/Applications/STM32CubeIDE_*"
    "C:/ST/STM32CubeIDE_*"
    "$ENV{ProgramFiles}/STMicroelectronics/STM32Cube/STM32CubeIDE_*"
)

find_program(
  STM32CUBEIDE_EXECUTABLE
  NAMES stm32cubeide STM32CubeIDE
  HINTS
    "$ENV{STM32CUBEIDE}"
    ${_cubeide_hints}
  PATHS
    "/usr/local/bin"
    "/usr/bin"
    "$ENV{HOME}/STMicroelectronics"
    "C:/ST"
    "$ENV{ProgramFiles}/STMicroelectronics"
    "$ENV{ProgramFiles\(x86\)}/STMicroelectronics"
  PATH_SUFFIXES
    "bin"
    "STM32CubeIDE"
    "STM32CubeIDE/bin"
    ""
  DOC "STM32CubeIDE executable (set STM32CUBEIDE env var to the install directory if not found)"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  STM32CubeIDE
  FOUND_VAR STM32CubeIDE_FOUND
  REQUIRED_VARS STM32CUBEIDE_EXECUTABLE
)

get_filename_component(
  CUBEIDE_DIR
  ${STM32CUBEIDE_EXECUTABLE}
  DIRECTORY
)

mark_as_advanced(STM32CUBEIDE_EXECUTABLE)

# Export the CubeIDE directory so the toolchain can locate bundled tools (e.g. OpenOCD)
set(CUBEIDE_DIR "${CUBEIDE_DIR}" PARENT_SCOPE)
