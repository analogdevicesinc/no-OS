find_program(
  STM32CUBEMX_EXECUTABLE
  NAMES STM32CubeMX stm32cubemx
  PATHS
    "/usr/local/bin"
    "/usr/bin"
    "/opt/st"
    "/opt/STMicroelectronics"
    "/usr/local/STMicroelectronics"
    "$ENV{HOME}/st"
    "$ENV{HOME}/STMicroelectronics"
    "$ENV{HOME}/STM32CubeMX"
    "C:/ST"
    "$ENV{ProgramFiles}/STMicroelectronics"
    "$ENV{ProgramFiles\(x86\)}/STMicroelectronics"
    "$ENV{ProgramFiles}/STMicroelectronics/STM32Cube/STM32CubeMX"
    "/opt/STMicroelectronics/STM32Cube/STM32CubeMX"
    "/usr/local/STMicroelectronics/STM32Cube/STM32CubeMX"
    "$ENV{HOME}/STMicroelectronics/STM32Cube/STM32CubeMX"
    "$ENV{HOME}/STM32CubeMX"
  PATH_SUFFIXES
    "STM32Cube/STM32CubeMX"
    "STM32CubeMX"
    "bin"
  DOC "STM32CubeMX executable"
)

find_program(
  STM32CUBEMX_JAVA
  NAMES java
  HINTS
    "${STM32CUBEMX_EXECUTABLE}/bin/jre"
  DOC "STM32CubeMX java executable"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  STM32CubeMX
  FOUND_VAR STM32CUBEMX_FOUND
  REQUIRED_VARS STM32CUBEMX_EXECUTABLE
)

get_filename_component(
  CUBEMX_DIR
  ${STM32CUBEMX_EXECUTABLE}
  DIRECTORY
)

mark_as_advanced(STM32CUBEMX_EXECUTABLE)