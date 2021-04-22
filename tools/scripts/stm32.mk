ifndef STM32CUBE
$(error STM32CUBE not found in shell environment.$(ENDL))
endif

PLATFORM_RELATIVE_PATH = $(patsubst $(STM32CUBE)%,stm32%,$1)
PLATFORM_FULL_PATH = $(patsubst stm32%,$(STM32CUBE)%,$1)
CREATED_DIRECTORIES += stm32

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

$(PROJECT_TARGET):
	$(MUTE) $(call mk_dir, $(BUILD_DIR))
	$(MUTE) $(call set_one_time_rule,$@)

