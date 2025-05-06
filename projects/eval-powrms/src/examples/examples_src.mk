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

