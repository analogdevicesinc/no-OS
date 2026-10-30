message(STATUS "Altera / Intel FPGA platform")

# This toolchain file supports both the legacy Altera/Nios II style flow and
# the newer Nios V / Intel FPGA BSP-based flow. The selected mode can come
# from Kconfig (CONFIG_ALTERA_PLATFORM_NIOSV) or from the exported
# ALTERA_PLATFORM_NIOSV environment variable. For Nios V BSP mode, the
# toolchain uses the standard CMake compiler variables directly.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR niosv)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(_altera_use_niosv OFF)
if(DEFINED CONFIG_ALTERA_PLATFORM_NIOSV AND "${CONFIG_ALTERA_PLATFORM_NIOSV}" STREQUAL "ON")
    set(_altera_use_niosv ON)
elseif(DEFINED ENV{ALTERA_PLATFORM_NIOSV} AND NOT "$ENV{ALTERA_PLATFORM_NIOSV}" STREQUAL "")
    set(_altera_use_niosv ON)
elseif(DEFINED BOARD AND "${BOARD}" STREQUAL "altera")
    set(_altera_use_niosv ON)
endif()

set(_altera_cc "")
if(DEFINED ENV{CMAKE_C_COMPILER} AND NOT "$ENV{CMAKE_C_COMPILER}" STREQUAL "")
    set(_altera_cc "$ENV{CMAKE_C_COMPILER}")
elseif(DEFINED CMAKE_C_COMPILER AND NOT CMAKE_C_COMPILER STREQUAL "")
    if(IS_ABSOLUTE "${CMAKE_C_COMPILER}" OR "${CMAKE_C_COMPILER}" MATCHES "/")
        set(_altera_cc "${CMAKE_C_COMPILER}")
    else()
        message(FATAL_ERROR "Altera toolchain requires a full path in CMAKE_C_COMPILER; got '${CMAKE_C_COMPILER}'.")
    endif()
endif()

if(_altera_use_niosv AND (NOT DEFINED ENV{ALTERA_BSP_DIR} OR "$ENV{ALTERA_BSP_DIR}" STREQUAL "") AND NOT DEFINED ALTERA_BSP_DIR)
    message(STATUS "Altera toolchain: no ALTERA_BSP_DIR was supplied; the BSP will be resolved by the Altera SDK integration during configure")
endif()

if(_altera_cc STREQUAL "")
    message(STATUS "Altera toolchain: CMAKE_C_COMPILER was not supplied; relying on CMake's compiler detection")
else()
    set(CMAKE_C_COMPILER "${_altera_cc}" CACHE FILEPATH "C compiler" FORCE)
endif()

# CMAKE_LINKER/CMAKE_SIZE/CMAKE_OBJCOPY/CMAKE_OBJDUMP are resolved from
# CMAKE_C_COMPILER later, in cmake/altera/altera_platform_sdk.cmake's
# config_altera_sdk() — CMake only turns them into build commands at
# generate time, so resolving them here as well would just recompute the
# same paths from the same compiler.

set(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections -std=gnu11 -Wall -Wextra" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "-ffunction-sections -fdata-sections -Wall -Wextra" CACHE STRING "C++ compiler flags" FORCE)
set(CMAKE_ASM_FLAGS "-D__ASSEMBLY__" CACHE STRING "ASM compiler flags" FORCE)

if(DEFINED ENV{ALTERA_CFLAGS} AND NOT "$ENV{ALTERA_CFLAGS}" STREQUAL "")
    string(APPEND CMAKE_C_FLAGS " $ENV{ALTERA_CFLAGS}")
    string(APPEND CMAKE_CXX_FLAGS " $ENV{ALTERA_CFLAGS}")
    string(APPEND CMAKE_ASM_FLAGS " $ENV{ALTERA_CFLAGS}")
endif()

if(DEFINED ENV{ALTERA_LDFLAGS} AND NOT "$ENV{ALTERA_LDFLAGS}" STREQUAL "")
    set(CMAKE_EXE_LINKER_FLAGS "$ENV{ALTERA_LDFLAGS}" CACHE STRING "Linker flags" FORCE)
endif()

message(STATUS "Configured Altera toolchain mode: ${_altera_use_niosv}")
message(STATUS "  compiler: ${_altera_cc}")
if(_altera_use_niosv)
    if(DEFINED ALTERA_BSP_DIR AND NOT "${ALTERA_BSP_DIR}" STREQUAL "")
        message(STATUS "  BSP directory: ${ALTERA_BSP_DIR}")
    else()
        message(STATUS "  BSP directory: resolved later by the Altera SDK integration")
    endif()
endif()
