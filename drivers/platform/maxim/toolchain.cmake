message(STATUS "MAXIM_LIBRARIES: $ENV{MAXIM_LIBRARIES}")

if(DEFINED ENV{MAXIM_LIBRARIES})
    set(MAXIM_LIBRARIES $ENV{MAXIM_LIBRARIES})
else()
    message(FATAL_ERROR "MAXIM_LIBRARIES is not defined")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

include("/mnt/drive/testing/no-OS/drivers/platform/maxim/max32690/memory_layout.cmake")

if (${USE_VENDOR_TOOLCHAIN})

# Specify the cross compiler
set(CMAKE_C_COMPILER ${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin/arm-none-eabi-gcc)
set(CMAKE_LINKER ${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin/arm-none-eabi-ld)

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

set(OPENOCD_PATH ${MAXIM_LIBRARIES}/../Tools/OpenOCD)