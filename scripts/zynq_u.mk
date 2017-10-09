# 8series zynq

ifeq ($(HDF-FILE),)
  HDF-FILE := $(M_HDF_FILE)
endif

ifeq ($(OS), Windows_NT)
  XSCT_CMD := xsct.bat 
  XSDB_CMD := xsdb.bat
else
  XSCT_CMD := xsct 
  XSDB_CMD := xsdb 
endif

CPU := ZYNQ_PSU
XSCT_LOG := xsct.log
XSCT_SCRIPT := $(NOOS-DIR)/scripts/xsct.tcl $(CPU)
XSDB_SCRIPT := $(NOOS-DIR)/scripts/xsdb.tcl $(CPU)
XSDB_CAPTURE := $(NOOS-DIR)/scripts/capture.tcl $(CPU)

COMPILER_DEFINES := XILINX
COMPILER_DEFINES += ZYNQ
COMPILER_DEFINES += $(CPU)
COMPILER_DEFINES += $(M_DEFINES)
COMPILER_DEFINES += $(subst $(comma),$(space),$(DEFINE))

P_HDR_FILES := xilsw/src/platform_config.h
P_HDR_FILES += xilsw/src/platform.h
P_SRC_FILES := xilsw/src/platform.c

ELF_FILE := sw/Release/sw.elf

HDR_FILES := $(P_HDR_FILES)
HDR_FILES += $(foreach i_dir, $(M_INC_DIRS), $(wildcard $(i_dir)/*.h))
HDR_FILES += $(foreach i_dir, $(M_SRC_DIRS), $(wildcard $(i_dir)/*.h))

SRC_FILES := $(P_SRC_FILES)
SRC_FILES += $(M_SRC_FILES)
SRC_FILES += $(foreach i_dir, $(M_SRC_DIRS), $(wildcard $(i_dir)/*.c))

CAPTURE_BADDR := 800000
CAPTURE_SIZE := 32768

ifeq ($(REBUILD),1)
  $(shell rm -fr .metadata .Xil hw fsbl_bsp fsbl xilsw sw xsct.log)
endif

.PHONY: all
all: $(ELF_FILE)


$(ELF_FILE): $(HDR_FILES) $(SRC_FILES)
	$(XSCT_CMD) $(XSCT_SCRIPT) sources $(HDR_FILES) $(SRC_FILES) >> $(XSCT_LOG) 2>&1
	$(XSCT_CMD) $(XSCT_SCRIPT) build


$(P_HDR_FILES): hw/system_top.bit
$(P_SRC_FILES): hw/system_top.bit


hw/system_top.bit: $(HDF-FILE)
	rm -fr .metadata .Xil hw fsbl_bsp fsbl xilsw sw xsct.log
	$(XSCT_CMD) $(XSCT_SCRIPT) init $(HDF-FILE) > $(XSCT_LOG) 2>&1
	$(XSCT_CMD) $(XSCT_SCRIPT) defines $(COMPILER_DEFINES) >> $(XSCT_LOG) 2>&1
	$(XSCT_CMD) $(XSCT_SCRIPT) make-bsp-xilsw >> $(XSCT_LOG) 2>&1


.PHONY: run
run: $(ELF_FILE)
	$(XSDB_CMD) $(XSDB_SCRIPT)

.PHONY: clean
clean: 
	rm -rf hw fsbl_bsp fsbl sw .metadata .Xil xilsw xsct.log SDK.log


.PHONY: capture
capture: $(ELF_FILE)
	$(XSDB_CMD) $(XSDB_CAPTURE) $(CAPTURE_BADDR) $(CAPTURE_SIZE)

BOOT.bin: hw/system_top.bit fsbl/Release/fsbl.elf $(ELF_FILE)
	echo "image:" > zynq.bif
	echo "{" >> zynq.bif
	echo "\t[bootloader]fsbl/Release/fsbl.elf" >> zynq.bif
	echo "\thw/system_top.bit" >> zynq.bif
	echo "\tsw/Release/sw.elf" >> zynq.bif
	echo "}" >> zynq.bif
	bootgen -image zynq.bif -arch zynqmp -o BOOT.bin

