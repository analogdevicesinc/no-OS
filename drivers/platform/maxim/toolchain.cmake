message(STATUS "MAXIM_LIBRARIES: $ENV{MAXIM_LIBRARIES}")

if(DEFINED ENV{MAXIM_LIBRARIES})
    set(MAXIM_LIBRARIES $ENV{MAXIM_LIBRARIES})
else()
    message(FATAL_ERROR "MAXIM_LIBRARIES is not defined")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)  # Target architecture

# Specify the cross compiler
set(CMAKE_C_COMPILER ${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin/arm-none-eabi-gcc)
set(CMAKE_LINKER ${MAXIM_LIBRARIES}/../Tools/GNUTools/10.3/bin/arm-none-eabi-ld)

set(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g3")
set(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_EXE_LINKER_FLAGS "-mthumb -mcpu=cortex-m4 -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections -mfpu=fpv4-sp-d16")

message(STATUS "MAXIM_LIBRARIES in toolchain: ${MAXIM_LIBRARIES}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32650/Source/GCC/max32650.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --entry=Reset_Handler")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")

set(OPENOCD_PATH ${MAXIM_LIBRARIES}/../Tools/OpenOCD)