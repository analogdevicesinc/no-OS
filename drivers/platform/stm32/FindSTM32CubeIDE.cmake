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
endif()
