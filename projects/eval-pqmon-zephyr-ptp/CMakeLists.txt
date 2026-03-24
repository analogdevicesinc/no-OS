cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(pqmon_zephyr)

# DTS binding for MAX31343 RTC
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# USB compat shims (must come BEFORE HAL includes so wrap_max32_usb.h
# and mcr_regs.h are found before the upstream versions)
target_include_directories(zephyr_interface BEFORE INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/usb_compat)

# Include directories
zephyr_include_directories(drivers)
zephyr_include_directories(lib/pqlib/include)
zephyr_include_directories(src)
zephyr_include_directories(src/iio)

# Zephyr PTP subsystem internals (clock.h, port.h, ds.h, state_machine.h)
# Needed by ptp_status.c to query PTP port state and sync accuracy
target_include_directories(app PRIVATE ${ZEPHYR_BASE}/subsys/net/lib/ptp)

# Application sources
target_sources(app PRIVATE
    src/main.c
    src/no_os_shim.c
    src/afe_config.c
    src/afe_calibration.c
    src/flash_storage.c
    src/pqlib_afe.c
    src/pqlib_example.c
    src/pqlib_convert.c
    src/ptp_clock_adin1110_zephyr.c
    src/adin1110_early_init.c
    src/rtc_max31343.c
    src/iio/iiod.c
    src/iio_pqm.c
    src/iio_server.c
    src/ptp_status.c
    src/afe_sync.c
    src/uart_export.c
)

# Link PQLib static library
target_link_libraries(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/pqlib/libadi_pqlib_cm4_gcc.a
)
