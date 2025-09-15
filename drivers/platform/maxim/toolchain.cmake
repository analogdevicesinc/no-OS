message(STATUS "MAXIM_LIBRARIES: $ENV{MAXIM_LIBRARIES}")

if(DEFINED ENV{MAXIM_LIBRARIES})
    set(MAXIM_LIBRARIES $ENV{MAXIM_LIBRARIES})
else()
    message(FATAL_ERROR "MAXIM_LIBRARIES is not defined")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

include("${CMAKE_CURRENT_LIST_DIR}/max32690/memory_layout.cmake")

if (${USE_VENDOR_TOOLCHAIN})

set(CROSS_COMPILER_BIN "${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin")

find_program(CMAKE_C_COMPILER arm-none-eabi-gcc
             HINTS ${CROSS_COMPILER_BIN})

find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++
             HINTS ${CROSS_COMPILER_BIN})

find_program(CMAKE_CXX_COMPILER arm-none-eabi-gcc
             HINTS ${CROSS_COMPILER_BIN})

find_program(CMAKE_LINKER arm-none-eabi-ld
             HINTS ${CROSS_COMPILER_BIN})

else()

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_LINKER arm-none-eabi-ld)

endif()

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -MD -g3")
set(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_EXE_LINKER_FLAGS "-mthumb -mcpu=cortex-m4 -specs=nosys.specs -Wl,--gc-sections -mfpu=fpv4-sp-d16 ${MCU_LINKER_FLAGS} \
                            -T${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC/${TARGET}.ld                    \
                            --entry=Reset_Handler" CACHE STRING "Linker flags for MCU" FORCE)

message(STATUS "MAXIM_LIBRARIES in toolchain: ${MAXIM_LIBRARIES}")

set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")

# set(OPENOCD_PATH ${MAXIM_LIBRARIES}/../Tools/OpenOCD)

file(GLOB_RECURSE OPENOCD_PATH ${MAXIM_LIBRARIES}/../Tools/*/openocd)
list(GET OPENOCD_PATH 0 OPENOCD_PATH)

# Set OpenOCD scripts directory using cmake_path for cross-platform compatibility
cmake_path(SET OPENOCD_SCRIPTS NORMALIZE "${MAXIM_LIBRARIES}/../Tools/OpenOCD/scripts")

# OpenOCD configuration for debugging
set(OPENOCD_INTERFACE "interface/cmsis-dap.cfg")
set(OPENOCD_CHIPNAME ${TARGET})
set(OPENOCD_TARGETCFG "target/${TARGET}.cfg")