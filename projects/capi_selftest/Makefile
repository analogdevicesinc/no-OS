# Select the example you want to enable
EXAMPLE ?= basic

# Select the hardware file (XSA for Xilinx)
HARDWARE ?= zed_gic.xsa

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

ifeq ($(PLATFORM),xilinx)
PHONY += capi_selftest_prepare_xilinx_spec
xilinx_project: capi_selftest_prepare_xilinx_spec

capi_selftest_prepare_xilinx_spec:
	@if [ -e "$(BUILD_DIR)/app/src/Xilinx.spec" ]; then \
		chmod u+w "$(BUILD_DIR)/app/src/Xilinx.spec"; \
	fi
endif

include ../../tools/scripts/generic.mk

ifeq ($(PLATFORM),xilinx)
FSBL_PATH = $(TEMP_DIR)/output/hw0/export/hw0/sw/boot/fsbl.elf
endif
