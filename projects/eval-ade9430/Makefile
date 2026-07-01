EXAMPLE ?= iot_example

# Enable the ADE9430 hardware reset pin sequence (required on the EVAL-PQMON
# board). Leave disabled for boards without a reset line wired to the MCU.
HW_RESET ?= n
ifeq (y,$(strip $(HW_RESET)))
CFLAGS += -DHW_RESET_ENABLED
endif

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
