# Uncomment to use the desired platform
# PLATFORM = xilinx

EXAMPLE ?= iio

# Select part number to use: ad4695, ad4696*, ad4697, ad4698.
AD469X_DEV ?= ad4696
# Select to enable temperature channel by choosing y.
AD469X_TEMP ?= n
# Select channel sequencer method: standard*, advanced, single.
AD469X_SEQ ?= standard

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
