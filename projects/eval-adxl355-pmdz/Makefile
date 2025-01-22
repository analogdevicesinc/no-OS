EXAMPLE ?= iio_trigger_example

# Select the device you want to enable by choosing y for enabling and n for disabling
ADXL355 = y
ADXL357 = n
ADXL359 = n

ifeq (y,$(strip $(ADXL355)))
CFLAGS += -DADXL355_DEV
else ifeq (y,$(strip $(ADXL357)))
CFLAGS += -DADXL357_DEV
else
CFLAGS += -DADXL359_DEV
endif

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
