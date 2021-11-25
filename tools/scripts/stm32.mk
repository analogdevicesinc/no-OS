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

#Set PATH variables where used binaries are found
#COMPILER_BIN = $(CCES_HOME)/ARM/gcc-arm-embedded/bin
#OPENOCD_SCRIPTS = $(CCES_HOME)/ARM/openocd/share/openocd/scripts
#OPENOCD_BIN = $(CCES_HOME)/ARM/openocd/bin
#CCES_EXE = $(CCES_HOME)/Eclipse

#export PATH := $(CCES_EXE):$(OPENOCD_SCRIPTS):$(OPENOCD_BIN):$(COMPILER_BIN):$(PATH)

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1

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

	$(call print,Configuring project)
	$(MUTE) $(STM32CUBEMX)/$(IDE) -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
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
	--specs=nano.specs \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=hard \
	-mthumb \
	-mcpu=cortex-m4

LDFLAGS	= -mcpu=cortex-m4 \
	--specs=nosys.specs \
	-Wl,--gc-sections \
	-static \
	--specs=nano.specs \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=hard \
	-mthumb \
	-Wl,--start-group \
	-lc \
	-lm \
	-Wl,--end-group

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
AR = arm-none-eabi-ar

ifneq '' '$(call rwildcard,src,stm32f4*)'
CFLAGS += -I$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc \
	-I$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
	-I$(STM32CUBE)/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-I$(STM32CUBE)/STM32CubeF4/Drivers/CMSIS/Include
TARGETCFG = target/stm32f4x.cfg
else
$(error Couldn't detect stm32 family.$(ENDL))
endif

openocd_paths:
ifndef OPENOCD_SCRIPTS
	$(error OPENOCD_SCRIPTS not found in shell environment.$(ENDL))
endif

ifndef OPENOCD_BIN
	$(error OPENOCD_BIN not found in shell environment.$(ENDL))
endif

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

.PHONY: stm32_run
stm32_run: all openocd_paths $(BINARY).openocd 
	$(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" -f $(BINARY).openocd \
		-c "program $(BINARY) verify reset exit"

.PHONY: debug
debug: all openocd_paths $(BINARY).openocd $(BINARY).gdb
	($(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" -f $(BINARY).openocd \
		-c "init" &);
	arm-none-eabi-gdb --command=$(BINARY).gdb

