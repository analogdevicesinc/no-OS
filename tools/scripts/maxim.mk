ifndef MAXIM_LIBRARIES
$(error MAXIM_LIBRARIES not defined.$(ENDL))
endif

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1
CREATED_DIRECTORIES += Maxim

PROJECT_BUILD = $(BUILD_DIR)/app

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

CHIPNAME=max32660
TARGETCFG=$(CHIPNAME).cfg
HEX=$(basename $(BINARY)).hex

#OPENOCD_SCRIPTS=$(MAXIM_LIBRARIES)/../../../Toolchain/share/openocd/scripts
OPENOCD_SCRIPTS=/home/xvr/Documents/Maxim/sf_shared2/Toolchain/share/openocd/scripts

ifeq ($(OS),Windows_NT)
OPENOCD_BIN=$(MAXIM_LIBRARIES)/../../../Toolchain/bin
else
OPENOCD_BIN=$(dir $(shell which openocd))
endif

ifeq ($(OPENOCD_BIN),'')
$(error Openocd not found.$(ENDL))
endif

LDFLAGS = -mcpu=cortex-m4 	\
	-Wl,--gc-sections 	\
	--specs=nosys.specs	\
	-mfloat-abi=hard 	\
	-mfpu=fpv4-sp-d16 	\
	--entry=Reset_Handler		
	
CFLAGS=-mthumb                                                                 \
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

CFLAGS += -I$(MAXIM_LIBRARIES)/CMSIS/Include	\
	-I$(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Include	\
	-I$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include	\
	-I$(MAXIM_LIBRARIES)/Boards/EvKit_V1/Include		

CFLAGS += -DTARGET_REV=$(TARGET_REV) \
	-DTARGET=$(TARGET)		\
	-DMAXIM_PLATFORM

LSCRIPT = $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Source/GCC/max32660.ld

.PHONY: $(BINARY).openocd
$(BINARY).openocd:
	@echo -f interface/cmsis-dap.cfg > $(BINARY).openocd
	@echo -f target/$(TARGETCFG) >> $(BINARY).openocd

.PHONY: $(BINARY).gdb
$(BINARY).gdb:
	@echo target remote localhost:3333 > $(BINARY).gdb	
	@echo load $(BINARY) >> $(BINARY).gdb	
	@echo file $(BINARY) >> $(BINARY).gdb
	@echo b main >> $(BINARY).gdb	
	@echo monitor reset halt >> $(BINARY).gdb	
	@echo tui enable >> $(BINARY).gdb	
	@echo c >> $(BINARY).gdb	

$(HEX): $(BINARY)
	$(MUTE) $(call print,[HEX] $(notdir $@))
	$(MUTE) $(OC) -O ihex $(BINARY) $(HEX)
	$(MUTE) $(call print,$(notdit $@) is ready)

clean_hex:
	@$(call print,[Delete] $(HEX))
	-$(MUTE) $(call remove_fun,$(HEX)) $(HIDE)

clean: clean_hex

.PHONY: maxim_run
$(PLATFORM)_run: all $(BINARY).openocd
	$(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" -f $(BINARY).openocd \
		-c "program $(BINARY) verify reset exit"

.PHONY: debug
debug: all $(BINARY).openocd $(BINARY).gdb
	($(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" -f $(BINARY).openocd \
		-c "init" &);
	$(GDB) --command=$(BINARY).gdb


