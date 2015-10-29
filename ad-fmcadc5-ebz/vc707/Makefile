## Makefile
## Auto-generated - do not modify.

ifeq ($(HDF-FILE),)
  HDF-FILE := system_top.hdf
endif

SDK_CMD := xsct
SDK_LOG := xilinx_xsct.log
SDK_SCRIPT := ../../scripts/xilinx_xsct.tcl

CC_CMD_PREFIX = mb
CC_CMD = $(CC_CMD_PREFIX)-gcc
CC_FLAGS := -Wall -mlittle-endian -mxl-soft-mul -mcpu=v9.2 -mxl-soft-mul
CC_FLAGS += -DXILINX
CC_FLAGS += -Ibsp/sys_mb/include
CC_FLAGS += -I..
CC_FLAGS += -I../../common_drivers/xilinx_platform_drivers
CC_FLAGS += -I../../common_drivers/jesd204b_gt
CC_FLAGS += -I../../common_drivers/jesd204b_v51
CC_FLAGS += -I../../common_drivers/adc_core
CC_FLAGS += -I../../drivers/ad9625
CC_FLAGS += -Os
CC_FLAGS += -ffunction-sections
CC_FLAGS += -fdata-sections

P_HDR_FILE := sw/src/platform.h
P_SRC_FILE := sw/src/platform.c

LINKER_SCRIPT := sw/src/lscript.ld
LINKER_LIBRARY := bsp/sys_mb/lib/libxil.a
LINKER_LIBRARY_PATH := $(dir $(LINKER_LIBRARY))
LINKER_FLAGS := -L$(LINKER_LIBRARY_PATH)
LINKER_FLAGS += -T$(LINKER_SCRIPT)
LINKER_FLAGS += -Wl,--start-group,-lxil,-lgcc,-lc,--end-group

ELF_FILE := vc707.elf

HDR_FILES := $(P_HDR_FILE)
HDR_FILES += $(wildcard ../*.h)
HDR_FILES += $(wildcard ../../common_drivers/xilinx_platform_drivers/*.h)
HDR_FILES += $(wildcard ../../common_drivers/jesd204b_gt/*.h)
HDR_FILES += $(wildcard ../../common_drivers/jesd204b_v51/*.h)
HDR_FILES += $(wildcard ../../common_drivers/adc_core/*.h)
HDR_FILES += $(wildcard ../../drivers/ad9625/*.h)

SRC_FILES := $(P_SRC_FILE)
SRC_FILES += $(wildcard ../*.c)
SRC_FILES += $(wildcard ../../common_drivers/xilinx_platform_drivers/*.c)
SRC_FILES += $(wildcard ../../common_drivers/jesd204b_gt/*.c)
SRC_FILES += $(wildcard ../../common_drivers/jesd204b_v51/*.c)
SRC_FILES += $(wildcard ../../common_drivers/adc_core/*.c)
SRC_FILES += $(wildcard ../../drivers/ad9625/*.c)

all: $(ELF_FILE)


$(ELF_FILE): $(HDR_FILES) $(SRC_FILES) $(LINKER_SCRIPT) $(LINKER_LIBRARY)
	$(CC_CMD) $(CC_FLAGS) -o $@ $(SRC_FILES) $(LINKER_FLAGS)


$(P_SRC_FILE) $(P_HDR_FILE) $(LINKER_SCRIPT): $(SDK_SCRIPT) $(HDF-FILE)
	$(SDK_CMD) -s $(SDK_SCRIPT) $(HDF-FILE) >> $(SDK_LOG) 2>&1

