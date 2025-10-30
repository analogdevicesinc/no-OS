CFLAGS += -DSSD_1306_EXAMPLE
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
		${PROJECT}/src/examples/example/powrms_gpios.h
SRCS += ${PROJECT}/src/examples/example/powrms_utils.c					\
		${PROJECT}/src/examples/example/powrms_gpios.c

# IIO
INCS += ${PROJECT}/src/examples/example/iio_powrms.h
SRCS += ${PROJECT}/src/examples/example/iio_powrms.c

# Data processing
SRCS += ${PROJECT}/src/examples/example/powrms_data_processing.c
INCS += ${PROJECT}/src/examples/example/powrms_data_processing.h

# EEPROM
INCS += $(NO-OS)/drivers/eeprom/m24512/m24512.h
SRCS += $(NO-OS)/drivers/eeprom/m24512/m24512.c