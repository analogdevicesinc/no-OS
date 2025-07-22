find_program(
  STM32CUBEIDE_EXECUTABLE
  NAMES stm32cubeide
  HINTS
    "/home/xvr/st/stm32cubeide_1.16.0"
  DOC "STM32CubeIDE executable"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  STM32CubeIDE
  FOUND_VAR STM32CubeIDE_FOUND
  REQUIRED_VARS STM32CUBEIDE_EXECUTABLE
)

mark_as_advanced(STM32CUBEIDE_EXECUTABLE)