# Native host toolchain for the Linux platform.
#
# Unlike the embedded platforms (maxim, stm32, ...), the Linux platform builds a
# normal userspace executable that runs on the host/target (e.g. a Raspberry Pi
# 5) on top of the Linux kernel's spidev/gpiochip character-device interfaces.
# There is therefore NO cross-compilation, NO -mcpu/-mfpu, NO -specs=nosys, and
# NO linker script: the system compiler and linker are used as-is.
#
# When cross-building for a different Linux target (e.g. building on an x86 host
# for a Pi), pass a cross toolchain explicitly via -DCMAKE_C_COMPILER=... or a
# dedicated -DCMAKE_TOOLCHAIN_FILE=..., or build natively on the target.

find_program(CMAKE_C_COMPILER gcc)
find_program(CMAKE_CXX_COMPILER g++)
find_program(CMAKE_ASM_COMPILER gcc)
find_program(CMAKE_OBJCOPY objcopy)
find_program(CMAKE_SIZE size)

if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR
        "gcc not found on PATH. Install a C toolchain (e.g. 'sudo apt install build-essential') "
        "or point CMAKE_C_COMPILER at a compiler for the Linux target.")
endif()

# _GNU_SOURCE exposes the character-device ioctls used by the linux_* drivers;
# gnu11 keeps the GNU extensions the no-OS core relies on.
set(COMMON_C_FLAGS "-ffunction-sections -fdata-sections -D_GNU_SOURCE -std=gnu11 -MD")

set(CMAKE_C_FLAGS "${COMMON_C_FLAGS}" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "-ffunction-sections -fdata-sections -D_GNU_SOURCE -MD" CACHE STRING "C++ compiler flags" FORCE)
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp" CACHE STRING "ASM compiler flags" FORCE)

# Debug build flags - Full debug info, no optimization
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -DDEBUG" CACHE STRING "C compiler flags for Debug" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG" CACHE STRING "C++ compiler flags for Debug" FORCE)

# Release build flags - Optimize for speed, disable assertions
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C compiler flags for Release" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C++ compiler flags for Release" FORCE)

# RelWithDebInfo build flags - Optimize with debug info
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "C compiler flags for RelWithDebInfo" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "C++ compiler flags for RelWithDebInfo" FORCE)

# MinSizeRel build flags - Optimize for size
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C compiler flags for MinSizeRel" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C++ compiler flags for MinSizeRel" FORCE)

# Reclaim unused sections; the iio example additionally links pthread/rt for the
# TCP iiod server, which the project CMakeLists adds via target_link_libraries.
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections" CACHE STRING "Linker flags" FORCE)
