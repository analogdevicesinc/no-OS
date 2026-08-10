message(STATUS "macOS host platform")

# Host build on macOS.
# CMake will discover the host system compiler automatically (AppleClang or
# a Homebrew-installed GCC/Clang).  No cross-compilation toolchain is needed.
set(CMAKE_SYSTEM_NAME Darwin)

# Name the output <project>.elf for naming symmetry with the other host
# platforms (linux-userspace) and the embedded platforms (maxim/stm32/xilinx/
# pico), even though the produced binary is actually Mach-O, not ELF.
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")
