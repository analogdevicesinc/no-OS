ifeq ($(OS),Windows_NT)
$(error STM32 builds on Windows are not currently supported.)
else
IDE = stm32cubeide
STM32CUBEIDE ?= $(wildcard /opt/stm32cubeide/*)
ifeq ($(STM32CUBEIDE),)
$(error $(ENDL)$(ENDL)STM32CUBEIDE not found at /opt/stm32cubeide\
		$(ENDL)$(ENDL)\
		Please run command "export STM32CUBEIDE=/path/to/your/stm32cubeide"$(ENDL)\
		Ex: export STM32CUBEIDE=/opt/stm32cubeide$(ENDL)$(ENDL))
else
STM32CUBEIDE=/opt/stm32cubeide
endif # STM32CUBEIDE check

MX = STM32CubeMX
STM32CUBEMX ?= $(wildcard /opt/stm32cubemx/*)
ifeq ($(STM32CUBEMX),)
$(error $(ENDL)$(ENDL)STM32CUBEMX not found at /opt/stm32cubemx\
		$(ENDL)$(ENDL)\
		Please run command "export STM32CUBEMX=/path/to/your/stm32cubemx"$(ENDL)\
		Ex: export STM32CUBEMX=/opt/stm32cubemx$(ENDL)$(ENDL))
else
STM32CUBEMX=/opt/stm32cubemx
endif # STM32CUBEMX check
endif # OS check 

# Locate the compiler path under STM32CubeIDE plugins directory
COMPILER_BIN = $(realpath $(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *arm-none-eabi-gcc)))

# Locate openocd location under STM32CubeIDE plugins directory
OPENOCD_SCRIPTS = $(realpath $(addsuffix ..,$(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *st_scripts/interface/stlink-dap.cfg))))
OPENOCD_BIN = $(realpath $(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *bin/openocd)))

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1

# stm32 specific implementation of this function, sources need to go under Core directory
relative_to_project = $(addprefix $(PROJECT_BUILD)/Core/, $(call get_relative_path, $1))

# Add all .c files related to stm32 to PLATFORM_SRCS in full path
PLATFORM_SRCS += $(call rwildcard, $(PROJECT_BUILD)/Drivers, *.c)
PLATFORM_SRCS += $(call rwildcard, $(PROJECT_BUILD)/Core/Src, *.c)

# Add all relevant stm32 include directories with -I prefix
EXTRA_INCS += $(call rwildcard, $(PROJECT_BUILD)/Drivers, *.h)
EXTRA_INCS += $(call rwildcard, $(PROJECT_BUILD)/Core/Inc, *.h)
PLATFORM_INCS += $(sort $(foreach h,$(EXTRA_INCS), -I$(dir $h)))

# Get the path of the .s script
ASM_SRCS += $(call rwildcard, $(PROJECT_BUILD)/Core/Startup,*.s)

# Extract the TARGET and CHIPNAME from the .s script filename
include gmsl
TARGET=$(join $(call uc, $(call substr,$(notdir $(ASM_SRCS)),9,17)), xx)
CHIPNAME=$(join $(call uc, $(call substr,$(notdir $(ASM_SRCS)),9,20)), x)
TARGETCFG=$(join target/, $(join $(call substr,$(notdir $(ASM_SRCS)),9,15), x.cfg))
CFLAGS += -D$(TARGET)

# Get the path of the linker script 
LSCRIPT=$(wildcard $(PROJECT_BUILD)/*FLASH.ld)

$(PROJECT_TARGET):
	$(call print,Creating IDE project)
	$(MUTE) $(call mk_dir, $(BUILD_DIR))
	@echo config load $(HARDWARE) > $(BINARY).cubemx
	@echo project name app >> $(BINARY).cubemx
	@echo project toolchain STM32CubeIDE >> $(BINARY).cubemx
	@echo project path $(BUILD_DIR) >> $(BINARY).cubemx
	@echo SetCopyLibrary "copy all" >> $(BINARY).cubemx
	@echo SetStructure Advanced >> $(BINARY).cubemx
	@echo project generate >> $(BINARY).cubemx
	@echo exit >> $(BINARY).cubemx
	$(MUTE) java -jar $(STM32CUBEMX)/$(MX) -q $(BINARY).cubemx $(HIDE)
	-$(MUTE)$(call remove_fun,$(BINARY).cubemx) $(HIDE)

	$(MUTE) sed -i 's/ main(/ generated_main(/' $(PROJECT_BUILD)/Core/Src/main.c $(HIDE)
	$(MUTE) $(call copy_fun, $(PROJECT_BUILD)/Core/Src/main.c, $(PROJECT_BUILD)/Core/Src/generated_main.c) $(HIDE)
	$(MUTE) $(call remove_fun, $(PROJECT_BUILD)/Core/Src/main.c) $(HIDE)

	$(MUTE) $(call remove_fun, $(PROJECT_BUILD)/Core/Src/syscalls.c) $(HIDE)
	
	$(call print,Configuring project)
	$(MUTE) $(STM32CUBEIDE)/$(IDE) -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
		-import $(PROJECT_BUILD) -data $(BUILD_DIR) -nosplash $(HIDE)

	$(MUTE) $(call set_one_time_rule,$@)

develop:
	$(MUTE) $(STM32CUBEIDE)/$(IDE) -nosplash -import $(PROJECT_BUILD) -data $(BUILD_DIR) $(HIDE)

CFLAGS += -std=gnu11 \
	-g3 \
	-DUSE_HAL_DRIVER \
	-DDEBUG \
	-DSTM32_PLATFORM \
	-O0 \
	-ffunction-sections \
	-fdata-sections \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-mcpu=cortex-m4

LDFLAGS	= -mcpu=cortex-m4 \
	-Wl,--gc-sections \
	-static \
	--specs=nosys.specs \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=hard \
	-mthumb \
	-Wl,--start-group \
	-lc \
	-lm \
	-Wl,--end-group
 
AR = arm-none-eabi-ar
AS = arm-none-eabi-gcc
CC = arm-none-eabi-gcc
GDB = arm-none-eabi-gdb
OC = arm-none-eabi-objcopy

.PHONY: $(BINARY).openocd
$(BINARY).openocd:
	@echo source [find interface/stlink-dap.cfg] > $(BINARY).openocd
	@echo set WORKAREASIZE 0x8000 >> $(BINARY).openocd
	@echo transport select \"dapdirect_swd\" >> $(BINARY).openocd
	@echo set CHIPNAME $(CHIPNAME) >> $(BINARY).openocd
	@echo set ENABLE_LOW_POWER 1 >> $(BINARY).openocd
	@echo set STOP_WATCHDOG 1 >> $(BINARY).openocd
	@echo set CLOCK_FREQ 8000 >> $(BINARY).openocd
	@echo reset_config srst_only srst_nogate connect_assert_srst >> $(BINARY).openocd
	@echo set CONNECT_UNDER_RESET 1 >> $(BINARY).openocd
	@echo set CORE_RESET 0 >> $(BINARY).openocd
	@echo set AP_NUM 0 >> $(BINARY).openocd
	@echo set GDB_PORT 3333 >> $(BINARY).openocd
	@echo source [find $(TARGETCFG)] >> $(BINARY).openocd
	@echo tpiu config disable >> $(BINARY).openocd

.PHONY: $(BINARY).gdb
$(BINARY).gdb:
	@echo target remote localhost:3333 > $(BINARY).gdb
	@echo load $(BINARY) >> $(BINARY).gdb
	@echo file $(BINARY) >> $(BINARY).gdb
	@echo monitor reset halt >> $(BINARY).gdb
	@echo tb main >> $(BINARY).gdb
	@echo tb HardFault_Handler >> $(BINARY).gdb
	# These may be helpful but are disabled due to
	# limited number of breakpoints:
	# @echo tb UsageFault_Handler >> $(BINARY).gdb
	# @echo tb MemManage_Handler >> $(BINARY).gdb
	# @echo tb BusFault_Handler >> $(BINARY).gdb
	@echo tui enable >> $(BINARY).gdb
	@echo c >> $(BINARY).gdb

HEX = $(basename $(BINARY)).hex

$(HEX): $(BINARY)
	$(MUTE) $(call print,[HEX] $(notdir $@))
	$(MUTE) $(OC) -O ihex $(BINARY) $(HEX)
	$(MUTE) $(call print,$(notdir $@) is ready)

post_build: $(HEX)

clean_hex:
	@$(call print,[Delete] $(HEX))
	-$(MUTE) $(call remove_fun,$(HEX)) $(HIDE)

clean: clean_hex

.PHONY: stm32_run
stm32_run: all $(BINARY).openocd 
	openocd -s "$(OPENOCD_SCRIPTS)" -f $(BINARY).openocd \
		-c "program $(BINARY) verify reset exit"

.PHONY: debug
debug: all $(BINARY).openocd $(BINARY).gdb
	($(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" -f $(BINARY).openocd \
		-c "init" &);
	$(GDB) --command=$(BINARY).gdb

