CFLAGS += -DSSD_1306_EXAMPLE
# SSD1306 panel overrides. These must be compiler -D flags, not #defines in a
# project header: ssd_1306.c is its own translation unit and never includes
# common_data.h, so a header #define would never reach the driver and the
# in-driver #ifndef defaults would win.
#   SA0 strapped high -> panel answers on 0x3D instead of the 0x3C default.
#   Panel mounted upside-down -> rotate the image 180 degrees.
CFLAGS += -DSSD1306_I2C_ADDR=0x3D
CFLAGS += -DSSD1306_ROTATION=180
# Enable verbose logging: route pr_debug()/pr_info()/pr_err() to the debug UART.
CFLAGS += -DNO_OS_LOG_LEVEL=7
SRCS += $(PROJECT)/src/examples/example/example.c
INCS += $(PROJECT)/src/examples/example/example.h

# Display drivers
SRCS += $(NO-OS)/drivers/display/ssd_1306/ssd_1306.c 
SRCS += $(NO-OS)/drivers/display/display.c 
INCS += $(NO-OS)/drivers/display/ssd_1306/ssd_1306.h
INCS += $(NO-OS)/drivers/display/display.h

# ADC drivers
SRCS += $(NO-OS)/drivers/adc/ad7091r5/ad7091r5.c
INCS += $(NO-OS)/drivers/adc/ad7091r5/ad7091r5.h

# Screens
SRCS += ${PROJECT}/src/examples/screens/subscreen_main_menu_screen.c	\
		${PROJECT}/src/examples/screens/subscreen_show_screen.c			\
		${PROJECT}/src/examples/screens/subscreen_settings_screen.c		\
		${PROJECT}/src/examples/screens/subscreen_blank_screen.c		\
		${PROJECT}/src/examples/screens/global_screen.c

INCS += ${PROJECT}/src/examples/screens/subscreen_main_menu_screen.h	\
		${PROJECT}/src/examples/screens/subscreen_show_screen.h			\
		${PROJECT}/src/examples/screens/subscreen_settings_screen.h		\
		${PROJECT}/src/examples/screens/subscreen_blank_screen.h		\
		${PROJECT}/src/examples/screens/global_screen.h

# Utils
INCS += ${PROJECT}/src/examples/example/powrms_utils.h					\
		${PROJECT}/src/examples/example/powrms_gpios.h					\
		${PROJECT}/src/examples/example/powrms_max77986.h
SRCS += ${PROJECT}/src/examples/example/powrms_utils.c					\
		${PROJECT}/src/examples/example/powrms_gpios.c					\
		${PROJECT}/src/examples/example/powrms_max77986.c

# IIO
INCS += ${PROJECT}/src/examples/example/iio_powrms.h
SRCS += ${PROJECT}/src/examples/example/iio_powrms.c

# Data processing
SRCS += ${PROJECT}/src/examples/example/powrms_data_processing.c
INCS += ${PROJECT}/src/examples/example/powrms_data_processing.h

# EEPROM
INCS += $(NO-OS)/drivers/eeprom/m24512/m24512.h
SRCS += $(NO-OS)/drivers/eeprom/m24512/m24512.c