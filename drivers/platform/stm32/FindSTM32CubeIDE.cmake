find_program(
  STM32CUBEIDE_EXECUTABLE
  NAMES
    stm32cubeide_1.16.1
    stm32cubeide_1.16.0
    stm32cubeide_1.15.1
    stm32cubeide_1.15.0
    stm32cubeide_1.14.1
    stm32cubeide_1.14.0
    stm32cubeide_1.13.2
    stm32cubeide_1.13.1
    stm32cubeide_1.13.0
    stm32cubeide_1.12.1
    stm32cubeide_1.12.0
    stm32cubeide_1.11.2
    stm32cubeide_1.11.1
    stm32cubeide_1.11.0
    stm32cubeide_1.10.1
    stm32cubeide_1.10.0
    stm32cubeide        # Generic fallback
    STM32CubeIDE        # Windows variant
  PATHS
    "/usr/local/bin"
    "/usr/bin"
    "/opt/st"
    "$ENV{HOME}/st"
    "$ENV{HOME}/STMicroelectronics"
    "C:/ST"
    "$ENV{ProgramFiles}/STMicroelectronics"
    "$ENV{ProgramFiles\(x86\)}/STMicroelectronics"
  PATH_SUFFIXES
    "STM32CubeIDE"
    "STM32CubeIDE/bin"
    "bin"
  DOC "STM32CubeIDE executable (any supported version)"
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

file(GLOB_RECURSE OPENOCD_PATH ${CUBEIDE_DIR}/plugins/*/openocd)
file(GLOB_RECURSE OPENOCD_SCRIPTS ${CUBEIDE_DIR}/plugins/*/mem_helper.tcl)

if(OPENOCD_SCRIPTS)
  cmake_path(GET OPENOCD_SCRIPTS PARENT_PATH OPENOCD_SCRIPTS)

  # OpenOCD interface configuration
  set(OPENOCD_INTERFACE "interface/stlink.cfg")
endif()
