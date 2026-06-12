# This script patches the generated CMakeLists.txt from CubeMX.

# Read the contents of the generated file
file(READ ${CMAKE_FILE_TO_PATCH} FILE_CONTENTS)

# Replace all instances of the incorrect variable with the correct one
string(REPLACE "\${CMAKE_SOURCE_DIR}" "${STM32_PROJECT_BUILD}"
     PATCHED_CONTENTS "${FILE_CONTENTS}")

# Write the corrected content back to the file
file(WRITE ${CMAKE_FILE_TO_PATCH} "${PATCHED_CONTENTS}")

file(READ ${CMAKE_FILE_TO_PATCH} FILE_CONTENTS)

string(REPLACE "\${CMAKE_PROJECT_NAME}" "no-os"
     PATCHED_CONTENTS "${FILE_CONTENTS}")

file(WRITE ${CMAKE_FILE_TO_PATCH} "${PATCHED_CONTENTS}")

# Remove syscalls.c to avoid duplicate symbol conflicts with
# stm32_uart_stdio.c (matches the make build behavior in stm32.mk).
file(READ ${CMAKE_FILE_TO_PATCH} FILE_CONTENTS)
string(REGEX REPLACE "[^\n]*syscalls\\.c[^\n]*\n" ""
     PATCHED_CONTENTS "${FILE_CONTENTS}")
file(WRITE ${CMAKE_FILE_TO_PATCH} "${PATCHED_CONTENTS}")

# Move CubeMX application sources (hal_msp, interrupt handlers, startup)
# from no-os into the STM32_Drivers OBJECT library.
#
# The HAL defines weak stubs for HAL_UART_MspInit, SysTick_Handler etc.
# in the same OBJECT library.  With both the weak stubs and the strong
# overrides in the same OBJECT library, all objects land unconditionally
# on the final link line, and the strong symbols reliably win.
#
# Keeping these files in no-os (a static archive when built as STATIC, or
# an OBJECT library that may not propagate through the dependency chain)
# caused the linker to use the weak stubs instead.
file(READ ${CMAKE_FILE_TO_PATCH} FILE_CONTENTS)
string(REPLACE "target_sources(no-os PRIVATE \${MX_Application_Src})"
               "target_sources(STM32_Drivers PRIVATE \${MX_Application_Src})"
     PATCHED_CONTENTS "${FILE_CONTENTS}")
file(WRITE ${CMAKE_FILE_TO_PATCH} "${PATCHED_CONTENTS}")

# Remove the target_link_libraries(no-os ...) line that links CubeMX
# OBJECT libraries (STM32_Drivers, USB_Device_Library, etc.) to no-os.
# OBJECT libraries don't propagate other OBJECT library dependencies
# transitively, so these must be linked directly to the build target.
# stm32_platform_sdk.cmake handles this via MX_LINK_LIBS.
file(READ ${CMAKE_FILE_TO_PATCH} FILE_CONTENTS)
string(REGEX REPLACE "[^\n]*target_link_libraries\\(no-os [^\n]*MX_LINK_LIBS[^\n]*\n" ""
     PATCHED_CONTENTS "${FILE_CONTENTS}")
file(WRITE ${CMAKE_FILE_TO_PATCH} "${PATCHED_CONTENTS}")

file(READ "${STM32_PROJECT_BUILD}/Core/Src/main.c" MAIN_FILE_CONTENTS)

# Replace main function name with stm32_init
string(REPLACE "main(" "stm32_init("
     MAIN_PATCHED_CONTENTS "${MAIN_FILE_CONTENTS}")

# Remove the infinite loop that prevents stm32_init() from returning
# Replace the entire while(1) section with just a return statement
string(REPLACE "  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */" "  /* Initialization complete, return to caller */
  return 0;" MAIN_PATCHED_CONTENTS "${MAIN_PATCHED_CONTENTS}")

file(WRITE "${STM32_PROJECT_BUILD}/Core/Src/main.c" "${MAIN_PATCHED_CONTENTS}")

message(STATUS "Patched ${CMAKE_FILE_TO_PATCH} to use CMAKE_CURRENT_SOURCE_DIR.")