message(STATUS "Windows host platform")

# Host build on Windows.
# CMake will discover the host system compiler automatically:
#   - MSVC   when invoked from a Visual Studio developer prompt
#   - MinGW  when mingw-w64 gcc/g++ is on PATH (e.g. MSYS2)
#   - Clang  when llvm-clang is on PATH
#
# No cross-compilation toolchain is required.
set(CMAKE_SYSTEM_NAME Windows)
