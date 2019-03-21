# 7series zynq

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

ifeq ($(CAPTURE_BADDR),)
  CAPTURE_BADDR := 0x80800000
endif

ifeq ($(CAPTURE_SIZE),)
  CAPTURE_SIZE := 32768
endif

ifeq ($(NR_OF_CHAN),)
  NR_OF_CHAN := 1
endif

ifeq ($(BITS_PER_SAMPLE),)
  BITS_PER_SAMPLE := 16
endif

XSCT_LOG := xsct.log
XSCT_SCRIPT := $(NOOS-DIR)/scripts/xsct.tcl
XSDB_SCRIPT := $(NOOS-DIR)/scripts/xsdb.tcl
XSDB_CAPTURE := $(NOOS-DIR)/scripts/xilinx_capture.tcl

COMPILER_DEFINES := XILINX
COMPILER_DEFINES += MICROBLAZE
COMPILER_DEFINES += $(M_COMPILER_DEFINES)

P_HDR_FILES := xilsw/src/platform_config.h
P_HDR_FILES += xilsw/src/platform.h
P_SRC_FILES := xilsw/src/platform.c

ELF_FILE := sw/Release/sw.elf

HDR_FILES := $(P_HDR_FILES)
HDR_FILES += $(M_HDR_FILES)
HDR_FILES += $(foreach i_dir, $(M_INC_DIRS), $(wildcard $(i_dir)/*.h))

SRC_FILES := $(P_SRC_FILES)
SRC_FILES += $(M_SRC_FILES)
SRC_FILES += $(foreach i_dir, $(M_INC_DIRS), $(wildcard $(i_dir)/*.c))

LIB_FILES := $(M_LIB_FILES)
LIB_NAME := $(M_LIB_NAME)

.PHONY: all
all: $(ELF_FILE)


$(ELF_FILE): $(HDR_FILES) $(SRC_FILES) $(LIB_FILES)
	$(XSCT_CMD) $(XSCT_SCRIPT) sources $(HDR_FILES) $(SRC_FILES)  $(LIB_FILES) > $(XSCT_LOG) 2>&1
	$(XSCT_CMD) $(XSCT_SCRIPT) library $(LIB_NAME) >> $(XSCT_LOG) 2>&1
	$(XSCT_CMD) $(XSCT_SCRIPT) build


$(P_HDR_FILES): hw/system_top.bit
$(P_SRC_FILES): hw/system_top.bit


hw/system_top.bit: $(HDF-FILE)
	rm -fr .metadata .Xil hw bsp xilsw sw xsct.log
	$(XSCT_CMD) $(XSCT_SCRIPT) init $(HDF-FILE) > $(XSCT_LOG) 2>&1
	$(XSCT_CMD) $(XSCT_SCRIPT) defines $(HDF-FILE) $(COMPILER_DEFINES) >> $(XSCT_LOG) 2>&1
	$(XSCT_CMD) $(XSCT_SCRIPT) make-bsp-xilsw >> $(XSCT_LOG) 2>&1


.PHONY: run
run: $(ELF_FILE)
	$(XSDB_CMD) $(XSDB_SCRIPT) MICROBLAZE

.PHONY: clean
clean: 
	rm -rf hw bsp sw .metadata .Xil xilsw xsct.log SDK.log

.PHONY: capture
capture: $(ELF_FILE)
	$(XSDB_CMD) $(XSDB_CAPTURE) MICROBLAZE $(CAPTURE_BADDR) $(CAPTURE_SIZE) $(NR_OF_CHAN) $(BITS_PER_SAMPLE)

