# Pico (RP2040) platform SDK configuration.
#
# config_pico_sdk(BUILD_TARGET) is invoked by config_platform_sdk() (see
# cmake/project_utils.cmake) from the project's CMakeLists. The pico-sdk's own
# CMake (pulled in via pico_sdk_init() from the root CMakeLists) already provides
# the SDK headers, second-stage bootloader, linker script and flash region
# through its INTERFACE libraries, so this file only:
#   - links the SDK's stdlib/standard-link libraries directly onto the executable
#     (so boot2 and the memmap linker script attach to the final ELF),
#   - emits the OpenOCD configuration for flashing/debugging,
#   - optionally converts the ELF to UF2 with picotool.

function(config_pico_sdk BUILD_TARGET)
        # Link the full (source-bearing) pico-sdk libraries onto the executable.
        # They are linked here -- on the exe, not on the no-os OBJECT library --
        # so each SDK source is compiled exactly once (linking them on no-os would
        # compile them into no-os and again into the exe via propagation, causing
        # duplicate-symbol link errors). This pulls in, transitively:
        #   - pico_stdlib -> hardware_* + pico_runtime (SDK sources, defines),
        #   - pico_standard_link -> second-stage bootloader (bs2_default_library),
        #     the memmap_default.ld linker script, the generated pico_flash_region.ld
        #     and --gc-sections,
        #   - the libc/soft-float/printf --wrap options from the owning libs.
        # Plain signature to match the project's target_link_libraries(<exe> no-os).
        target_link_libraries(${BUILD_TARGET}
                pico_stdlib
                pico_stdio_uart
                hardware_spi
                hardware_i2c
                hardware_uart
                hardware_gpio
                hardware_irq
                hardware_timer
                hardware_clocks
                pico_printf
                pico_malloc
                pico_mem_ops
                pico_float
                pico_double
                pico_bit_ops
                pico_int64_ops
                pico_divider
                pico_standard_link)

        # OpenOCD configuration for the cmsis-dap (picoprobe) probe. Set as
        # locals and emit openocd.cfg here (mirroring config_stm32_sdk), since a
        # later generate_openocd_config() from post_build_config() runs in the
        # project scope where these are not visible.
        #
        # The "halted" event handler clears TIMER_DBGPAUSE (TIMER_BASE +0x2c) on
        # every core halt so busy_wait_*() loops do not spin forever under debug
        # (the RP2040 timer is paused while a core is halted by default).
        set(OPENOCD_INTERFACE "interface/cmsis-dap.cfg")
        set(OPENOCD_CHIPNAME "rp2040")
        set(OPENOCD_TARGETCFG "target/rp2040.cfg")
        set(OPENOCD_EXTRA_COMMANDS "adapter speed 5000")
        set(OPENOCD_POST_COMMANDS
                "rp2040.core0 configure -event halted { mww 0x4005402c 0 }")
        generate_openocd_config()

        # Optional UF2 image via picotool (not vendored). Skip with a status
        # message when unavailable, matching the Make build.
        find_program(PICOTOOL_PATH NAMES picotool DOC "Path to picotool")
        if(PICOTOOL_PATH)
                add_custom_command(
                        TARGET ${BUILD_TARGET}
                        POST_BUILD
                        COMMAND ${PICOTOOL_PATH} uf2 convert -t elf
                                $<TARGET_FILE:${BUILD_TARGET}>
                                -t uf2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_TARGET}.uf2
                                --family rp2040
                        COMMENT "Generating ${BUILD_TARGET}.uf2"
                        VERBATIM)
        else()
                message(STATUS
                        "picotool not found; the .uf2 image will not be generated "
                        "(.elf and .hex still build). Install picotool or set "
                        "-DPICOTOOL_PATH=... to enable it.")
        endif()
endfunction()
