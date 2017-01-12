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

XSCT_LOG := xsct.log
XSCT_SCRIPT := $(NOOS-DIR)/scripts/xsct.tcl
XSDB_SCRIPT := $(NOOS-DIR)/scripts/xsdb.tcl

CC_CMD_PREFIX = arm-none-eabi
CC_CMD = $(CC_CMD_PREFIX)-gcc
CC_FLAGS := -Wall
CC_FLAGS += -DXILINX
CC_FLAGS += -DZYNQ
CC_FLAGS += -DZYNQ_PS7
CC_FLAGS += -Ibsp/ps7_cortexa9_0/include
CC_FLAGS += -Isw/src
CC_FLAGS += $(addprefix -I, $(M_INC_DIRS))
CC_FLAGS += -O2
CC_FLAGS += -fmessage-length=0
CC_FLAGS += -mcpu=cortex-a9
CC_FLAGS += -mfpu=vfpv3
CC_FLAGS += -mfloat-abi=hard 
CC_FLAGS += -Wl,-build-id=none -specs=sw/src/Xilinx.spec

P_HDR_FILE := sw/src/platform.h
P_SRC_FILE := sw/src/platform.c

LINKER_SCRIPT := sw/src/lscript.ld
LINKER_LIBRARY := bsp/ps7_cortexa9_0/lib/libxil.a
LINKER_LIBRARY_PATH := $(dir $(LINKER_LIBRARY))
LINKER_FLAGS := -L$(LINKER_LIBRARY_PATH)
LINKER_FLAGS += -Wl,-T
LINKER_FLAGS += -Wl,$(LINKER_SCRIPT)
LINKER_FLAGS += -Wl,--start-group,-lxil,-lgcc,-lc,--end-group

ELF_FILE := sw.elf

HDR_FILES := $(P_HDR_FILE)
HDR_FILES += $(M_HDR_FILES)
HDR_FILES += $(wildcard $(NOOS-DIR)/common/*.h)
HDR_FILES += $(foreach i_dir, $(M_INC_DIRS), $(wildcard $(i_dir)/*.h))

SRC_FILES := $(P_SRC_FILE)
SRC_FILES += $(M_SRC_FILES)
SRC_FILES += $(wildcard $(NOOS-DIR)/common/*.c)
SRC_FILES += $(foreach i_dir, $(M_INC_DIRS), $(wildcard $(i_dir)/*.c))

all: $(ELF_FILE)


$(ELF_FILE): $(LINKER_LIBRARY) $(HDR_FILES) $(SRC_FILES) $(LINKER_SCRIPT) 
	$(CC_CMD) $(CC_FLAGS) -o $@ $(SRC_FILES) $(LINKER_FLAGS)


$(LINKER_LIBRARY): $(XSCT_SCRIPT) $(HDF-FILE)
	$(XSCT_CMD) $(XSCT_SCRIPT) $(HDF-FILE) > $(XSCT_LOG) 2>&1

run: $(ELF_FILE)
	$(XSDB_CMD) $(XSDB_SCRIPT) ZYNQ_PS7

