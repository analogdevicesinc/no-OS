message(STATUS "Linux Userspace Platform")

find_program(CMAKE_SIZE size)

# Native Linux build - use the host system compiler.
# No cross-compilation toolchain is required; CMake will discover the
# default system C/C++ compilers automatically.
set(CMAKE_SYSTEM_NAME Linux)

# Name the output <project>.elf, matching the embedded platforms
# (maxim/stm32/xilinx/pico) instead of CMake's default suffixless native
# executable name.
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# Needed for the POST_BUILD size report in project_utils.cmake; unlike the
# embedded toolchains this one never set it, so ${CMAKE_SIZE} expanded to
# nothing and the --format flag itself got run as a (nonexistent) command.
find_program(CMAKE_SIZE size)
