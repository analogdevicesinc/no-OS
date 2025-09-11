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
SRCS += ${PROJECT}/src/examples/screens/main_menu_screen.c		\
		${PROJECT}/src/examples/screens/startup_freq_screen.c	\
		${PROJECT}/src/examples/screens/show_screen.c			\
		${PROJECT}/src/examples/screens/settings_screen.c		\
		${PROJECT}/src/examples/screens/blank_screen.c

INCS += ${PROJECT}/src/examples/screens/main_menu_screen.h		\
		${PROJECT}/src/examples/screens/startup_freq_screen.h	\
		${PROJECT}/src/examples/screens/show_screen.h			\
		${PROJECT}/src/examples/screens/settings_screen.h		\
		${PROJECT}/src/examples/screens/blank_screen.h

# Utils
INCS += ${PROJECT}/src/examples/example/powrms_utils.h			\
		${PROJECT}/src/examples/example/powrms_variables.h		\
		${PROJECT}/src/examples/example/buttons.h
SRCS += ${PROJECT}/src/examples/example/powrms_utils.c			\
		${PROJECT}/src/examples/example/powrms_variables.c		\
		${PROJECT}/src/examples/example/buttons.c