# This script patches the generated CMakeLists.txt from CubeMX.

# Read the contents of the generated file
file(READ ${CMAKE_FILE_TO_PATCH} FILE_CONTENTS)

# Replace all instances of the incorrect variable with the correct one
string(REPLACE "\${CMAKE_SOURCE_DIR}" "${STM32_PROJECT_BUILD}"
     PATCHED_CONTENTS "${FILE_CONTENTS}")

# Write the corrected content back to the file
file(WRITE ${CMAKE_FILE_TO_PATCH} "${PATCHED_CONTENTS}")

file(READ "${STM32_PROJECT_BUILD}/Core/Src/main.c" MAIN_FILE_CONTENTS)

# Replace all instances of the incorrect variable with the correct one
string(REPLACE "main(" "stm32_init("
     MAIN_PATCHED_CONTENTS "${MAIN_FILE_CONTENTS}")

file(WRITE "${STM32_PROJECT_BUILD}/Core/Src/main.c" "${MAIN_PATCHED_CONTENTS}")

# Write the corrected content back to the file

message(STATUS "Patched ${CMAKE_FILE_TO_PATCH} to use CMAKE_CURRENT_SOURCE_DIR.")