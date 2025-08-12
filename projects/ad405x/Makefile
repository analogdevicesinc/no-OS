# Select the example you want to enable by choosing y for enabling and n for disabling
BASIC_EXAMPLE = y
BASIC_EXAMPLE_I3C = n
I3C_DMA_EXAMPLE = n
IIO_EXAMPLE = n

# Select the device you want to enable, the following are supported:
# AD4050, AD4052, AD4056, AD4058, AD4060, AD4062
DEV_TYPE = AD4052

# NUCLEO_H563ZI, NUCLEO_H503RB
CARRIER = NUCLEO_H563ZI

# Set the PID Instance ID to match the pins ADDR[2:0] (valid range: 0-7) (I3C only)
INSTANCE_ID = 0

include ../../tools/scripts/generic_variables.mk

include vars.mk

include src.mk

include ../../tools/scripts/generic.mk
