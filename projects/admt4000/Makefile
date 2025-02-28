# # Select the example measurement flows to include / enable
# ADMT4000_BASIC = n	# Basic Angles and Turns Measurement
# ADMT4000_DIAG = n	# Diagnostic Loop (i.e., temp, reference resistor etc.)
# ADMT4000_TEST = n	# Functional Configuration Test
# ADMT4000_ECC = n	# Demonstrate ECC computation flow
ADMT4000_IIO = y	# IIO compatible firmware (i.e., iio oscilloscope)

PLATFORM = mbed

EXAMPLE ?= iio_example

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk