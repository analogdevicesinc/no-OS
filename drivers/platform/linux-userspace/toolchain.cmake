message(STATUS "Linux Userspace Platform")

# Native Linux build - use the host system compiler.
# No cross-compilation toolchain is required; CMake will discover the
# default system C/C++ compilers automatically.
set(CMAKE_SYSTEM_NAME Linux)
