# Maxim RISC-V coprocessor build support for no-OS.
#
# Provides maxim_add_riscv_coprocessor(), which builds a RISC-V (CPU1) firmware
# image and embeds it into a primary ARM (CPU0) executable so the whole dual-core
# system ships as a single ELF. The RISC-V image runs in place from flash; the ARM
# core boots it by writing the image's flash entry address to FCR->urvbootaddr.
#
# A project enables this with one call:
#
#     include(maxim_coprocessor)
#     maxim_add_riscv_coprocessor(<arm_target>
#         SOURCES  <rv .c/.S ...>          # RISC-V firmware sources
#         INCLUDES <dir ...>               # extra RISC-V include dirs (optional)
#         DEFINES  <SYM[=val] ...>)        # extra RISC-V -D defines (optional)
#
# Everything else -- the two-pass link, the merged ARM linker script, the RISC-V
# startup/system/linker selection, and the .incbin embed -- is handled here.

include_guard(GLOBAL)

# Locate the RISC-V cross toolchain shipped with the Maxim SDK. Sets RISCV_GCC and
# RISCV_OBJCOPY in the caller's scope. The ARM toolchain file already resolved
# MAXIM_LIBRARIES; the RISC-V tools sit next to it under Tools/xPack.
function(maxim_find_riscv_toolchain)
    find_program(RISCV_GCC NAMES riscv-none-elf-gcc
        PATHS
            "${MAXIM_LIBRARIES}/../Tools/xPack/riscv-none-elf-gcc/*/bin"
            "${MAXIM_LIBRARIES}/../Tools/xPack/riscv-none-elf-gcc"
        NO_DEFAULT_PATH)
    if(NOT RISCV_GCC)
        find_program(RISCV_GCC NAMES riscv-none-elf-gcc)
    endif()
    if(NOT RISCV_GCC)
        message(FATAL_ERROR
            "maxim: riscv-none-elf-gcc not found. Install the Maxim SDK RISC-V "
            "tools or put riscv-none-elf-gcc on PATH.")
    endif()
    get_filename_component(_bindir ${RISCV_GCC} DIRECTORY)
    set(RISCV_GCC ${RISCV_GCC} PARENT_SCOPE)
    set(RISCV_OBJCOPY ${_bindir}/riscv-none-elf-objcopy PARENT_SCOPE)
endfunction()

function(maxim_add_riscv_coprocessor ARM_TARGET)
    cmake_parse_arguments(RV ""
        "FORCE_INCLUDE"
        "SOURCES;INCLUDES;DEFINES;DEPENDS;NOOS_DRIVERS;NOOS_INCLUDES" ${ARGN})
    if(NOT RV_SOURCES)
        message(FATAL_ERROR "maxim_add_riscv_coprocessor: SOURCES is required")
    endif()

    set(_gcc  ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC)
    set(_src  ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source)
    set(_bin  ${CMAKE_CURRENT_BINARY_DIR}/coprocessor)
    set(_arm_ld
        ${NO_OS_DIR}/drivers/platform/maxim/max${TARGET_NUM}/max${TARGET_NUM}_dualcore_arm.ld)
    file(MAKE_DIRECTORY ${_bin})

    maxim_find_riscv_toolchain()

    # RISC-V CPU1 is RV32IMC + Zicsr/Zifencei, ilp32 ABI.
    set(_march -march=rv32imc_zicsr_zifencei)
    set(_mabi  -mabi=ilp32)
    set(_incs -I${_gcc}
              -I${MAXIM_LIBRARIES}/CMSIS/Include
              -I${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Include
              -I${MAXIM_LIBRARIES}/PeriphDrivers/Include/MAX${TARGET_NUM}
              -I${MAXIM_LIBRARIES}/PeriphDrivers/Include/SYS)
    foreach(_d ${RV_INCLUDES})
        list(APPEND _incs -I${_d})
    endforeach()
    set(_defs -DTARGET=${TARGET_NUM} -DTARGET_REV=0x4131 -DRV32 -DRISCV)
    foreach(_d ${RV_DEFINES})
        list(APPEND _defs -D${_d})
    endforeach()
    set(_cflags ${_march} ${_mabi} -Os -ffunction-sections -fdata-sections
                ${_defs} ${_incs})

    # --- Optional: cross-compile no-OS drivers into the RISC-V image ----------
    # When NOOS_DRIVERS is given, the coprocessor uses the real no-OS driver
    # stack (e.g. no_os_spi + a device driver) instead of bare-metal register
    # code. These extra sources compile with the same RV flags plus the no-OS
    # include dirs and the generated no_os_config.h force-include (matching the
    # Arm no-os target). FORCE_INCLUDE optionally prepends one more -include
    # header into every RV translation unit (e.g. to patch SDK definitions).
    set(_noos_cflags ${_cflags})
    if(RV_NOOS_DRIVERS)
        list(APPEND _noos_cflags
             -I${NO_OS_DIR}/include
             -I${NO_OS_DIR}/drivers/platform/maxim/max${TARGET_NUM}
             -I${NO_OS_DIR}/drivers/platform/maxim/common
             -I${CMAKE_BINARY_DIR}/build          # generated no_os_config.h
             -include no_os_config.h)
        # MXC PeriphDrivers compile against private *_reva.h headers that live
        # next to each peripheral's source (e.g. Source/DMA/dma_reva.h included by
        # Source/SPI/spi_reva1.c), so add every Source/<PERIPH> dir as an include
        # path -- same as the Arm no-os build does.
        set(_mxc_src ${MAXIM_LIBRARIES}/PeriphDrivers/Source)
        list(APPEND _noos_cflags
             -I${_mxc_src}/SPI -I${_mxc_src}/GPIO -I${_mxc_src}/DMA
             -I${_mxc_src}/SYS -I${_mxc_src}/FLC)
        foreach(_d ${RV_NOOS_INCLUDES})
            list(APPEND _noos_cflags -I${_d})
        endforeach()
    endif()
    if(RV_FORCE_INCLUDE)
        # Applies to the firmware sources AND the no-OS drivers so the forced
        # header reaches every RV translation unit.
        list(APPEND _cflags -include ${RV_FORCE_INCLUDE})
        list(APPEND _noos_cflags -include ${RV_FORCE_INCLUDE})
    endif()

    # --- Link the ARM target with the dual-core ARM linker script -------------
    # It confines the ARM stack/heap to SRAM0/SRAM1 (out of the RISC-V's SRAM2)
    # and reserves the .riscv_flash region whose _riscv_boot symbol marks the
    # embedded image. Setting CMAKE_EXE_LINKER_FLAGS here (the caller's directory
    # scope) applies to both the ARM target and the premap twin below.
    string(REPLACE
        "-T${_gcc}/max${TARGET_NUM}.ld"
        "-T${_arm_ld} -L${_gcc}"
        _flags "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${_flags}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${_flags}")

    # --- Pass 1: premap ------------------------------------------------------
    # The RISC-V must be linked to run at its flash load address, but that address
    # (_riscv_boot) is only known after the ARM is linked. Build a twin of the ARM
    # executable from the same sources/includes (minus the yet-to-exist RISC-V
    # blob) purely to read _riscv_boot from its map. The address is stable with or
    # without the blob because .riscv_flash sits after all ARM content.
    get_target_property(_arm_srcs ${ARM_TARGET} SOURCES)
    get_target_property(_arm_incs ${ARM_TARGET} INCLUDE_DIRECTORIES)
    set(_premap ${ARM_TARGET}_premap)
    set(_premap_map ${_bin}/${_premap}.map)
    add_executable(${_premap} ${_arm_srcs})
    if(_arm_incs)
        target_include_directories(${_premap} PRIVATE ${_arm_incs})
    endif()
    target_link_libraries(${_premap} no-os)
    target_link_options(${_premap} PRIVATE -Wl,-Map=${_premap_map})
    add_custom_command(TARGET ${_premap} POST_BUILD
        BYPRODUCTS ${_premap_map}
        COMMAND ${CMAKE_COMMAND} -E true
        COMMENT "coprocessor: ${_premap}.map ready")

    # --- Pass 1b: derive the RISC-V flash origin from the premap map ----------
    # Writes coprocessor/buildrv/common_riscv.ld (__FlashStart/__FlashLength),
    # which the stock max${TARGET_NUM}_riscv.ld INCLUDEs.
    set(_common_ld ${_bin}/buildrv/common_riscv.ld)
    add_custom_command(OUTPUT ${_common_ld}
        COMMAND ${CMAKE_COMMAND}
            -DMAP_FILE=${_premap_map}
            -DOUT_FILE=${_common_ld}
            -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen_common_riscv_ld.cmake
        DEPENDS ${_premap} ${_premap_map}
                ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen_common_riscv_ld.cmake
        COMMENT "coprocessor: deriving RISC-V flash origin from ${_premap}.map")

    # --- Pass 2: compile the RISC-V objects ----------------------------------
    # Firmware sources + the stock RISC-V startup and system init (which set up the
    # C runtime and leave clock setup to the ARM core). Firmware compiles with the
    # no-OS include set when NOOS_DRIVERS is used (it calls the no-OS API), else
    # the lean flags.
    if(RV_NOOS_DRIVERS)
        set(_app_cflags ${_noos_cflags})
    else()
        set(_app_cflags ${_cflags})
    endif()
    set(_objs "")
    set(_n 0)
    foreach(_s ${RV_SOURCES})
        set(_o ${_bin}/rv_app_${_n}.o)
        add_custom_command(OUTPUT ${_o}
            COMMAND ${RISCV_GCC} ${_app_cflags} -c ${_s} -o ${_o}
            DEPENDS ${_s} ${RV_FORCE_INCLUDE} ${RV_DEPENDS} ${CONFIG_HEADER_FILE}
            COMMENT "coprocessor: compiling ${_s}")
        list(APPEND _objs ${_o})
        math(EXPR _n "${_n}+1")
    endforeach()

    # no-OS / MXC / device driver sources cross-compiled for RV32.
    foreach(_s ${RV_NOOS_DRIVERS})
        set(_o ${_bin}/rv_drv_${_n}.o)
        add_custom_command(OUTPUT ${_o}
            COMMAND ${RISCV_GCC} ${_noos_cflags} -c ${_s} -o ${_o}
            DEPENDS ${_s} ${CONFIG_HEADER_FILE}
            COMMENT "coprocessor: compiling (no-OS) ${_s}")
        list(APPEND _objs ${_o})
        math(EXPR _n "${_n}+1")
    endforeach()

    set(_startup_obj ${_bin}/startup_riscv.o)
    add_custom_command(OUTPUT ${_startup_obj}
        COMMAND ${RISCV_GCC} ${_march} ${_mabi} ${_incs}
            -c ${_gcc}/startup_riscv_max${TARGET_NUM}.S -o ${_startup_obj}
        DEPENDS ${_gcc}/startup_riscv_max${TARGET_NUM}.S
        COMMENT "coprocessor: assembling RISC-V startup")

    set(_system_obj ${_bin}/system_riscv.o)
    add_custom_command(OUTPUT ${_system_obj}
        COMMAND ${RISCV_GCC} ${_cflags}
            -c ${_src}/system_riscv_max${TARGET_NUM}.c -o ${_system_obj}
        DEPENDS ${_src}/system_riscv_max${TARGET_NUM}.c
        COMMENT "coprocessor: compiling RISC-V system init")

    # Bare-metal _sbrk: the SDK's heap.c uses __HeapBase/__HeapLimit symbols from
    # the linker script instead of Linux ecall stubs. Without it, any call to
    # malloc/printf (via newlib's -lc_nano) issues an 'ecall' that traps immediately
    # on bare metal because there is no OS to handle sys_brk.
    set(_heap_obj ${_bin}/heap_riscv.o)
    add_custom_command(OUTPUT ${_heap_obj}
        COMMAND ${RISCV_GCC} ${_cflags}
            -c ${_src}/heap.c -o ${_heap_obj}
        DEPENDS ${_src}/heap.c
        COMMENT "coprocessor: compiling bare-metal heap/_sbrk")

    # --- Pass 2b: link the RISC-V ELF to run in place from flash --------------
    set(_rv_elf ${_bin}/riscv.elf)
    set(_rv_bin ${_bin}/riscv.bin)
    add_custom_command(OUTPUT ${_rv_elf}
        COMMAND ${RISCV_GCC} ${_march} ${_mabi}
            -T${_gcc}/max${TARGET_NUM}_riscv.ld
            -L${_bin} -L${_gcc}
            -nostartfiles -Wl,--gc-sections -Wl,-Map=${_bin}/riscv.map
            # Redirect MXC_SYS_Reset_Periph to the RMW wrapper defined in the
            # application sources.  The SDK's plain-write implementation clobbers
            # unrelated bits in GCR->rst1 (SMPHR, SIMO) when resetting SPI0,
            # which corrupts the hardware semaphore and can glitch the JTAG TAP.
            -Wl,--wrap=MXC_SYS_Reset_Periph
            -o ${_rv_elf}
            ${_startup_obj} ${_system_obj} ${_heap_obj} ${_objs}
            -Wl,--start-group -lc_nano -lm -lgcc -Wl,--end-group
        DEPENDS ${_startup_obj} ${_system_obj} ${_heap_obj} ${_objs} ${_common_ld}
        COMMENT "coprocessor: linking RISC-V ELF (in-place flash)")

    add_custom_command(OUTPUT ${_rv_bin}
        COMMAND ${RISCV_OBJCOPY} -O binary ${_rv_elf} ${_rv_bin}
        DEPENDS ${_rv_elf}
        COMMENT "coprocessor: RISC-V ELF -> raw binary")

    # --- Pass 3: wrap the binary into riscv.o and link it into the ARM ELF -----
    # The wrapper object must be named riscv.o: the ARM linker script matches it by
    # name (KEEP(*riscv.o (.text*)) in .riscv_flash, EXCLUDE_FILE(*riscv.o) in
    # .text) to place the blob at _riscv_boot and keep it out of ARM code.
    set(RISCV_BIN ${_rv_bin})
    set(_loader_s ${_bin}/riscv_loader.S)
    configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates/riscv_loader.S.in
                   ${_loader_s} @ONLY)
    set(_embed_obj ${_bin}/riscv.o)
    add_custom_command(OUTPUT ${_embed_obj}
        COMMAND ${CMAKE_C_COMPILER} -x assembler-with-cpp -c ${_loader_s} -o ${_embed_obj}
        DEPENDS ${_rv_bin} ${_loader_s}
        COMMENT "coprocessor: embedding RISC-V binary")

    target_sources(${ARM_TARGET} PRIVATE ${_embed_obj})
    add_custom_target(${ARM_TARGET}_coprocessor DEPENDS ${_embed_obj})
    add_dependencies(${ARM_TARGET} ${ARM_TARGET}_coprocessor)
endfunction()
