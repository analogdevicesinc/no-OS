ifndef MAXIM_LIBRARIES
$(error MAXIM_LIBRARIES not defined.$(ENDL))
endif

UNIX_TOOLS_PATH := $(MAXIM_LIBRARIES)/../Tools/MinGW/msys/1.0/bin
ARM_COMPILER_PATH := $(realpath $(dir $(call rwildcard, $(MAXIM_LIBRARIES)/../Tools/GNUTools/, *bin/arm-none-eabi-gcc)))

export PATH := $(ARM_COMPILER_PATH):$(PATH)
ifeq ($(OS),Windows_NT)
export PATH := $(UNIX_TOOLS_PATH):$(PATH)
# Workaround for an issue related to this one: https://savannah.gnu.org/bugs/?35323
$(shell tr --version)
endif

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1
CREATED_DIRECTORIES += Maxim
PROJECT_BUILD = $(BUILD_DIR)/app
COMPILER = gcc

ifneq "$(HEAP_SIZE)" ""
CFLAGS+=-D__HEAP_SIZE=$(HEAP_SIZE)
endif
ifneq "$(STACK_SIZE)" ""
CFLAGS+=-D_STACK_SIZE=$(STACK_SIZE)
endif

CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-gcc
GDB=arm-none-eabi-gdb
OC=arm-none-eabi-objcopy

TARGET?=max32660
TARGET_NUMBER:=$(word 2,$(subst x, ,$(TARGET)))
TARGET_UCASE=$(addprefix MAX,$(TARGET_NUMBER))
TARGET_LCASE=$(addprefix max,$(TARGET_NUMBER))

PLATFORM_DRIVERS := $(NO-OS)/drivers/platform/maxim/$(TARGET)

ifeq ($(TARGET), max32655)
include $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/$(TARGET_UCASE)/Source/GCC/$(TARGET)_memory.mk
endif
include $(MAXIM_LIBRARIES)/PeriphDrivers/$(TARGET_LCASE)_files.mk

HEX=$(basename $(BINARY)).hex
TARGET_REV=0x4131

TARGETCFG=$(TARGET_LCASE).cfg

OPENOCD_SCRIPTS=$(MAXIM_LIBRARIES)/../Tools/OpenOCD/scripts
OPENOCD_BIN=$(MAXIM_LIBRARIES)/../Tools/OpenOCD

LDFLAGS = -mcpu=cortex-m4 	\
	-Wl,--gc-sections 	\
	--specs=nosys.specs	\
	-mfloat-abi=hard 	\
	-mfpu=fpv4-sp-d16 	\
	--entry=Reset_Handler		
	
CFLAGS += -mthumb                                                                 \
        -mcpu=cortex-m4                                                         \
        -mfloat-abi=hard                                                        \
        -mfpu=fpv4-sp-d16                                                       \
        -Wa,-mimplicit-it=thumb                                                 \
        -fsingle-precision-constant                                             \
        -ffunction-sections                                                     \
        -fdata-sections                                                         \
        -MD                                                                     \
        -Wall                                                                   \
        -Wdouble-promotion                                                      \
        -Wno-format                                                      \
	-g3									\
	-c	

ASFLAGS += -x assembler-with-cpp

ASFLAGS += $(PROJ_AFLAGS)
CFLAGS += -DTARGET_REV=$(TARGET_REV) \
	-DTARGET=$(TARGET)		\
	-DMAXIM_PLATFORM		\
	-DTARGET_NUM=$(TARGET_NUMBER)

SRC_TMP = $(foreach src,$(PERIPH_DRIVER_C_FILES),$(word 2,$(subst PeriphDrivers/, ,$(src))))
DRIVER_C_FILES = $(foreach src,$(SRC_TMP),$(addprefix $(MAXIM_LIBRARIES)/PeriphDrivers/,$(src)))

PLATFORM_SRCS += $(DRIVER_C_FILES)
INCLUDE_DIR_TMP = $(foreach src,$(PERIPH_DRIVER_INCLUDE_DIR),$(word 2,$(subst PeriphDrivers/, ,$(src))))
DRIVER_INCLUDE_DIR = $(foreach src,$(INCLUDE_DIR_TMP),$(addprefix $(MAXIM_LIBRARIES)/PeriphDrivers/,$(src)))
INCS += $(foreach dir,$(DRIVER_INCLUDE_DIR), $(wildcard $(dir)/*.h))

LSCRIPT = $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/$(TARGET_UCASE)/Source/GCC/$(TARGET_LCASE).ld
ASM_SRCS += $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/$(TARGET_UCASE)/Source/GCC/startup_$(TARGET_LCASE).S
PLATFORM_SRCS += $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/$(TARGET_UCASE)/Source/heap.c
PLATFORM_SRCS += $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/$(TARGET_UCASE)/Source/system_$(TARGET_LCASE).c 
INCS += $(wildcard $(MAXIM_LIBRARIES)/CMSIS/Include/*.h)
INCS += $(wildcard $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/$(TARGET_UCASE)/Include/*.h)

ifeq ($(TARGET), max32650)
INCS := $(filter-out $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/$(TARGET_UCASE)/Include/mxc_device.h, $(INCS))
endif

$(PROJECT_TARGET):
	$(call print, Building for target $(TARGET_LCASE))
	$(call print,Creating IDE project)
	$(MUTE) $(call mk_dir,$(BUILD_DIR)) $(HIDE)
	$(MUTE) $(call set_one_time_rule,$@)

$(PLATFORM)_sdkopen:
	$(shell python3 $(PLATFORM_TOOLS)/run_config.py $(NO-OS) $(BINARY) $(PROJECT) $(MAXIM_LIBRARIES) $(TARGET_LC) $(ARM_COMPILER_PATH))
	$(MUTE) code $(PROJECT)

$(PLATFORM)_sdkclean: clean

$(PLATFORM)_sdkbuild: build

.PHONY: $(BINARY).gdb
$(BINARY).gdb:
	@echo target remote localhost:3333 > $(BINARY).gdb	
	@echo load $(BINARY) >> $(BINARY).gdb	
	@echo file $(BINARY) >> $(BINARY).gdb
	@echo b main >> $(BINARY).gdb	
	@echo monitor reset halt >> $(BINARY).gdb	
ifneq ($(OS),Windows_NT)
	@echo tui enable >> $(BINARY).gdb
endif	
	@echo c >> $(BINARY).gdb	

$(HEX): $(BINARY)
	$(MUTE) $(call print,[HEX] $(notdir $@))
	$(MUTE) $(OC) -O ihex $(BINARY) $(HEX)
	$(MUTE) $(call print,$(notdir $@) is ready)

post_build: $(HEX)

clean_hex:
	@$(call print,[Delete] $(HEX))
	-$(MUTE) $(call remove_fun,$(HEX)) $(HIDE)

clean: clean_hex

.PHONY: maxim_run
$(PLATFORM)_run: all 
	$(OPENOCD_BIN)/openocd -s $(OPENOCD_SCRIPTS) 		\
		-f interface/cmsis-dap.cfg -f target/$(TARGETCFG) \
		-c "program $(BINARY) verify reset exit"

.PHONY: debug
debug: all $(BINARY).gdb start_openocd
	$(GDB) --command=$(BINARY).gdb

.PHONY: start_openocd
ifeq ($(OS),Windows_NT)
start_openocd:
	start $(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" 		\
		-f interface/cmsis-dap.cfg -f target/$(TARGETCFG)
else
start_openocd:
	$(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" 	\
		-f interface/cmsis-dap.cfg -f target/$(TARGETCFG) -c "init" &
endif
