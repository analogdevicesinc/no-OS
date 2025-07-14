# This script patches the generated CMakeLists.txt from CubeMX.

# Read the contents of the generated file
file(READ ${CMAKE_FILE_TO_PATCH} FILE_CONTENTS)

# Replace all instances of the incorrect variable with the correct one
string(REPLACE "\${CMAKE_SOURCE_DIR}" "/mnt/drive/testing/no-OS/build/test_build/app"
     PATCHED_CONTENTS "${FILE_CONTENTS}")

# Write the corrected content back to the file
file(WRITE ${CMAKE_FILE_TO_PATCH} "${PATCHED_CONTENTS}")

message(STATUS "Patched ${CMAKE_FILE_TO_PATCH} to use CMAKE_CURRENT_SOURCE_DIR.")