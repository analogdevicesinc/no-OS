add_executable(capi_coprocessor_max78000)

target_sources(capi_coprocessor_max78000 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

target_include_directories(capi_coprocessor_max78000 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(capi_coprocessor_max78000 no-os)

# Starting CPU1 gives the RISC-V core its MAX78000 SRAM banks. Keep the ARM
# data, heap, and stack in SRAM0/SRAM1 just like the existing dual-core demos;
# the default single-core linker script places the ARM stack in CPU1's SRAM3.
set(_maxim_gcc
    ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC)
set(_maxim_dualcore_ld
    ${NO_OS_DIR}/drivers/platform/maxim/max${TARGET_NUM}/max${TARGET_NUM}_dualcore_arm.ld)
string(REPLACE
    "-T${_maxim_gcc}/max${TARGET_NUM}.ld"
    "-T${_maxim_dualcore_ld} -L${_maxim_gcc}"
    CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

config_platform_sdk(capi_coprocessor_max78000)
add_flash_target(capi_coprocessor_max78000)
post_build_config(capi_coprocessor_max78000)
