add_executable(eval-powrms)

# Common sources
target_sources(eval-powrms PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
    # LVGL font used unconditionally by the display screens.
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_font_terminus_16.c
)

if(CONFIG_EVAL_POWRMS_EXAMPLE_EXAMPLE)
    target_sources(eval-powrms PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example/example.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example/iio_powrms.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example/powrms_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example/powrms_gpios.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example/powrms_max77986.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example/powrms_data_processing.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/screens/subscreen_main_menu_screen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/screens/subscreen_show_screen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/screens/subscreen_settings_screen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/screens/subscreen_blank_screen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/screens/global_screen.c
    )
    target_include_directories(eval-powrms PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/example
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/screens
    )
endif()

# SSD1306 panel overrides. These must be compiler -D flags, not #defines in a
# project header: ssd_1306.c is its own translation unit and never includes
# common_data.h, so a header #define would never reach the driver and the
# in-driver #ifndef defaults would win.
#   SA0 strapped high -> panel answers on 0x3D instead of the 0x3C default.
#   Panel mounted upside-down -> rotate the image 180 degrees.
target_compile_definitions(eval-powrms PRIVATE
    SSD_1306_EXAMPLE
    SSD1306_I2C_ADDR=0x3D
    SSD1306_ROTATION=180
    # Enable verbose logging: route pr_debug()/pr_info()/pr_err() to the debug UART.
    NO_OS_LOG_LEVEL=7
)

target_include_directories(eval-powrms PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# The application drives the MAX32662 windowed watchdog directly through the
# Maxim SDK (powrms_utils.c: MXC_WDT_*). The no-os maxim platform layer has no
# WDT wrapper, so pull the SDK WDT peripheral sources into the project target.
# MAX32662 is a "me12" part and its WDT block is the revb IP.
if(PLATFORM STREQUAL "maxim")
    target_sources(eval-powrms PRIVATE
        ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WDT/wdt_me12.c
        ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WDT/wdt_revb.c
    )
    target_include_directories(eval-powrms PUBLIC
        ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WDT
    )
endif()

target_link_libraries(eval-powrms no-os lvgl)

config_platform_sdk(eval-powrms)
add_flash_target(eval-powrms)
post_build_config(eval-powrms)
