EXAMPLE ?= basic

# Select the device you want to enable, the following are supported:
# AD4050, AD4052, AD4056, AD4058, AD4060, AD4062
DEV_TYPE ?= AD4052

# nucleo_h563zi.ioc, nucleo_h503rb.ioc
HARDWARE ?= carrier/nucleo-h563zi.ioc

# Set the PID Instance ID to match the pins ADDR[2:0] (valid range: 0-7) (I3C only)
INSTANCE_ID ?= 0

include ../../tools/scripts/generic_variables.mk

include vars.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
