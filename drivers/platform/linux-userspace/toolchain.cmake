message(STATUS "Linux Userspace Platform")

# Native Linux build - use the host system compiler.
# No cross-compilation toolchain is required; CMake will discover the
# default system C/C++ compilers automatically.
set(CMAKE_SYSTEM_NAME Linux)

# Name the output <project>.elf, matching the embedded platforms
# (maxim/stm32/xilinx/pico) instead of CMake's default suffixless native
# executable name.
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")
