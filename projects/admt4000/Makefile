EXAMPLE ?= iio_trigger_example
FIRMWARE_VERSION ?= 1.0.0.a.0
# COMMIT_VERSION ?= "123"
#$(shell git rev-parse --short HEAD 2>/dev/null || echo "unknown")

PLATFORM ?= stm32

TMC_ENABLED ?= y

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk