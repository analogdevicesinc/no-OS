# Maxim (MSDK) CAPI backends. Gated on the neutral CONFIG_CAPI_<MODULE> symbol.
#
# One shared UART backend (maxim_capi_uart.c) and one shared IRQ backend
# (maxim_capi_irq.c) cover every MSDK target; the per-part differences are
# absorbed inside the C sources via TARGET_NUM-gated shims and #ifdef MXC_*
# guards, so a given build only emits the vectors/paths its part actually has.

no_os_sources_ifdef(CONFIG_CAPI_ALLOC ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_alloc.c)
no_os_sources_ifdef(CONFIG_CAPI_UART  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_uart.c)
no_os_sources_ifdef(CONFIG_CAPI_IRQ   ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_irq.c)
no_os_sources_ifdef(CONFIG_CAPI_TIME  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_time.c)
no_os_sources_ifdef(CONFIG_CAPI_GPIO  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_gpio.c)
no_os_sources_ifdef(CONFIG_CAPI_SPI   ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_spi.c)
no_os_sources_ifdef(CONFIG_CAPI_I2C   ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_i2c.c)
no_os_sources_ifdef(CONFIG_CAPI_DMA   ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_dma.c)
no_os_sources_ifdef(CONFIG_CAPI_PINT  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_pint.c)
no_os_sources_ifdef(CONFIG_CAPI_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_timer.c)
no_os_sources_ifdef(CONFIG_CAPI_WDT   ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_wdt.c)

# Secure Boot (SLA) support for MAX32690.
# When CONFIG_MAXIM_SECURE_BOOT=y and TARGET_NUM==32690, include the secure
# application header source. The header is placed by the SLA linker script
# (max32690_sla.ld) at 0x10000000–0x100001FF; the SBT tools (sign_app,
# build_scp_session) then sign and package the image. See toolchain.cmake
# for the corresponding linker script selection.
if(TARGET_NUM EQUAL 32690 AND CONFIG_MAXIM_SECURE_BOOT)
    no_os_sources_ifdef(CONFIG_MAXIM_SECURE_BOOT ${CMAKE_CURRENT_SOURCE_DIR}/max${TARGET_NUM}/sla_header_MAX32690.c)
    message(STATUS "Including SLA header for secure boot (max${TARGET_NUM})")
endif()

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ---- Board (BSP) layer discriminator ----
# TARGET_NUM (a max<n>.h macro) identifies the SoC but cannot separate boards
# built on the same part: AD-APARD32690-SL and MAX32690EVKIT are both
# TARGET_NUM==32690 yet route the console/spare UARTs to different instances.
# MSDK encodes board identity as a BOARD_* macro in each BSP's board.h, but that
# header is not on the CAPI include path, so mirror it from the no-OS BOARD
# cache variable here.
#
# Two macros are emitted per board, because MSDK's own tagging is incomplete:
#
#   BOARD_<MSDK>        - the exact macro that BSP's board.h defines, so code
#                         already written against MSDK names keeps working.
#                         Only 40 of MSDK's 64 BSPs define one at all, and the
#                         name is NOT unique across parts (MAX32650, MAX32655,
#                         MAX32662, MAX32665, MAX32672, MAX32690, MAX78000 and
#                         MAX78002 all spell their kit BOARD_EVKIT_V1), so it is
#                         only meaningful nested inside a TARGET_NUM block.
#   MAX_CAPI_BOARD_<BSP> - always defined, derived from the MSDK BSP directory
#                         name, and unique when paired with TARGET_NUM. Use this
#                         for boards MSDK leaves untagged.
#
# Adding a board = one row in MAXIM_BOARD_MAP: "<no-OS BOARD>:<BSP dir>:<MSDK
# macro or NONE>". The BSP dir must exist under $MSDK/Libraries/Boards/<PART>/.
set(MAXIM_BOARD_MAP
    "ad-apard32690-sl:APARD:BOARD_APARD"
    "max32690evkit:EvKit_V1:BOARD_EVKIT_V1"
    "max32690fthr:FTHR:BOARD_FTHR"
    "ad-swiot1l-sl:FTHR_APPS_A:BOARD_FTHR_APPS_A"
    "max32650fthr:FTHR_APPS_A:BOARD_FTHR_APPS_A"
    "max32655fthr:FTHR_Apps_P1:BOARD_FTHR_APPS_P1"
    "max32660fthr:EvKit_V1:NONE"
    "max32666fthr:FTHR2:BOARD_FTHR2"
    "max32670evkit:EvKit_V1:NONE"
    "max78000fthr:FTHR_RevA:BOARD_FTHR_REVA"
)
# Notes on the rows that are not a clean one-to-one:
# Every board additionally gets a MAX_CAPI_BOARD_<no-OS BOARD> macro derived
# from its (always-unique) no-OS BOARD string, so a board can be identified even
# when it shares a BSP with another. Gate genuinely board-specific routing on
# that macro, not on the BSP-derived one, whenever the BSP is shared.
#   ad-swiot1l-sl - a MAX32650 design with no BSP of its own in MSDK. Mapped to
#                  FTHR_APPS_A so the build resolves; its own pin routing is NOT
#                  modelled. FTHR_APPS_A is shared with max32650fthr, so gate
#                  board-specific code on MAX_CAPI_BOARD_AD_SWIOT1L_SL (the
#                  unique no-OS-board macro), not MAX_CAPI_BOARD_FTHR_APPS_A.
#   max32660fthr - MSDK ships no FTHR BSP for MAX32660 (only Emulator and
#                  EvKit_V1), and EvKit_V1/board.h defines no BOARD_ macro.
#                  Mapped to EvKit_V1 so the build resolves; the FTHR's actual
#                  pin routing is NOT modelled. EvKit_V1 is shared, so gate on
#                  MAX_CAPI_BOARD_MAX32660FTHR and treat the routing as
#                  unverified until checked on hardware.
#   max32670evkit - same situation: the BSP exists but defines no BOARD_ macro.
#                  Gate on MAX_CAPI_BOARD_MAX32670EVKIT.
#   max32666fthr - MAX32666 is a build alias of MAX32665; FTHR2 is the dual-core
#                  Feather BSP. Unverified on hardware.

if(DEFINED BOARD)
    set(_maxim_board_found FALSE)
    foreach(_row IN LISTS MAXIM_BOARD_MAP)
        string(REPLACE ":" ";" _f "${_row}")
        list(GET _f 0 _noos)
        list(GET _f 1 _bsp)
        list(GET _f 2 _msdk)
        if(BOARD STREQUAL "${_noos}")
            string(TOUPPER "${_bsp}" _bsp_uc)
            target_compile_definitions(no-os PUBLIC MAX_CAPI_BOARD_${_bsp_uc})
            # Also emit a discriminator derived from the no-OS BOARD name itself.
            # The BSP-derived macro above is NOT unique -- ad-swiot1l-sl and
            # max32650fthr both resolve to FTHR_APPS_A, and several kits share
            # EvKit_V1 -- so it cannot separate two boards on the same BSP. The
            # no-OS BOARD string is always unique, so MAX_CAPI_BOARD_<BOARD> is
            # the macro to gate genuinely board-specific routing on (e.g.
            # MAX_CAPI_BOARD_AD_SWIOT1L_SL vs MAX_CAPI_BOARD_MAX32650FTHR).
            string(MAKE_C_IDENTIFIER "${BOARD}" _noos_id)
            string(TOUPPER "${_noos_id}" _noos_uc)
            target_compile_definitions(no-os PUBLIC MAX_CAPI_BOARD_${_noos_uc})
            if(NOT _msdk STREQUAL "NONE")
                target_compile_definitions(no-os PUBLIC ${_msdk})
            endif()
            message(STATUS "Maxim board: ${BOARD} -> BSP ${_bsp} "
                           "(MAX_CAPI_BOARD_${_bsp_uc}, MAX_CAPI_BOARD_${_noos_uc}, "
                           "MSDK ${_msdk})")
            set(_maxim_board_found TRUE)
            break()
        endif()
    endforeach()
    if(NOT _maxim_board_found)
        # Not an error: non-Maxim boards (coraz7s, nucleo-f767zi, ...) reach this
        # file only when a Maxim CAPI backend is somehow enabled for them. Warn
        # so a genuinely new Maxim board is not silently left without a
        # discriminator -- the failure mode is board-specific wiring quietly
        # compiling out, which is much harder to spot than a build error.
        message(WARNING
            "BOARD='${BOARD}' has no row in MAXIM_BOARD_MAP; no BOARD_* or "
            "MAX_CAPI_BOARD_* macro will be defined. If this is a Maxim board, "
            "add a row in capi/platform/maxim/CMakeLists.txt.")
    endif()
endif()
