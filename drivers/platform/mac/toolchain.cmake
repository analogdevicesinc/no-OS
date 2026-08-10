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

# Needed for the POST_BUILD size report in project_utils.cmake; unlike the
# embedded toolchains this one never set it, so ${CMAKE_SIZE} expanded to
# nothing and the --format flag itself got run as a (nonexistent) command.
# macOS's own `size` (cctools) doesn't understand GNU's --format=berkeley;
# project_utils.cmake omits that flag on APPLE for this reason.
find_program(CMAKE_SIZE size)
