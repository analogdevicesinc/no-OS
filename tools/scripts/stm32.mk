ifeq ($(OS),Windows_NT)
$(error STM32 builds on Windows are not currently supported.)
else
IDE = stm32cubeide
STM32CUBEIDE ?= $(wildcard /opt/stm32cubeide)
ifeq ($(STM32CUBEIDE),)
$(error $(ENDL)$(ENDL)STM32CUBEIDE not defined or not found at default path /opt/stm32cubeide\
		$(ENDL)$(ENDL)\
		Please run command "export STM32CUBEIDE=/path/to/your/stm32cubeide"$(ENDL)\
		Ex: export STM32CUBEIDE=/opt/stm32cubeide$(ENDL)$(ENDL))
endif # STM32CUBEIDE check

MX = STM32CubeMX
STM32CUBEMX ?= $(wildcard /opt/stm32cubemx)
ifeq ($(STM32CUBEMX),)
$(error $(ENDL)$(ENDL)STM32CUBEMX not defined or not found at default path /opt/stm32cubemx\
		$(ENDL)$(ENDL)\
		Please run command "export STM32CUBEMX=/path/to/your/stm32cubemx"$(ENDL)\
		Ex: export STM32CUBEMX=/opt/stm32cubemx$(ENDL)$(ENDL))
endif # STM32CUBEMX check
endif # OS check

# Locate the compiler path under STM32CubeIDE plugins directory
COMPILER_BIN = $(realpath $(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *arm-none-eabi-gcc)))

# Locate openocd location under STM32CubeIDE plugins directory
OPENOCD_SCRIPTS = $(realpath $(addsuffix ..,$(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *st_scripts/interface/stlink-dap.cfg))))
OPENOCD_BIN = $(realpath $(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *bin/openocd)))

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1

# stm32 specific build directory tree (project goes under app, but user .c/.h sources must go under app/Core)
PROJECT_BUILDROOT = $(BUILD_DIR)/app
PROJECT_BUILD = $(PROJECT_BUILDROOT)/Core
RELEASE_DIR = $(PROJECT_BUILDROOT)/Release
DEBUG_DIR = $(PROJECT_BUILDROOT)/Debug

# Add all .c files related to stm32 to PLATFORM_SRCS in full path
PLATFORM_SRCS += $(call rwildcard, $(PROJECT_BUILDROOT)/Drivers, *.c)
PLATFORM_SRCS += $(call rwildcard, $(PROJECT_BUILD)/Src, *.c)

# Add all relevant stm32 include directories with -I prefix
EXTRA_INCS += $(call rwildcard, $(PROJECT_BUILDROOT)/Drivers, *.h)
EXTRA_INCS += $(call rwildcard, $(PROJECT_BUILD)/Inc, *.h)
PLATFORM_INCS += $(sort $(foreach h,$(EXTRA_INCS), -I$(dir $h)))

# Get the path of the .s script
ASM_SRCS += $(call rwildcard, $(PROJECT_BUILD)/Startup,*.s)

# Get the path of the interrupts file
ITC = $(call rwildcard, $(PROJECT_BUILD)/Src,*_it.c)

# Get the path of the hal config file
HALCONF = $(call rwildcard, $(PROJECT_BUILD)/Inc,*_hal_conf.h)

ifneq (,$(wildcard $(PROJECT_BUILD)))
TARGET = $(shell sed -rn 's|^.*(STM32[A-Z][0-9][0-9][0-9]x+)"/>$$|\1|p' $(PROJECT_BUILDROOT)/.cproject | head -n 1)
CFLAGS += -D$(TARGET)
CHIPNAME = $(shell sed -rn 's|^.*(STM32[A-Z][0-9][0-9][0-9][A-Z][A-Z][A-Z]x+)" .*$$|\1|p' $(PROJECT_BUILDROOT)/.cproject | head -n 1)
TARGETCFG = $(shell sed -rn "s|^.*(STM32[A-Z][0-9]).*$$|target/\L\1x.cfg|p" $(PROJECT_BUILDROOT)/.cproject | head -n 1)
endif

# Get the path of the linker script
LSCRIPT=$(wildcard $(PROJECT_BUILDROOT)/*FLASH.ld)

# Get the extra flags that need to be added into the .cproject file
CPROJECTFLAGS = $(sort $(subst -D,,$(filter -D%, $(CFLAGS))))

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
	$(MUTE) $(call remove_file,$(BINARY).cubemx) $(HIDE)
	$(MUTE) $(MAKE) --no-print-directory $(PROJECT_TARGET)_configure
	$(MUTE) $(call set_one_time_rule,$@)

$(PROJECT_TARGET)_configure:
	$(call print,Configuring project)
	$(MUTE) sed -i 's/ main(/ stm32_init(/' $(PROJECT_BUILD)/Src/main.c $(HIDE)
	$(MUTE) sed -i '0,/while (1)/s//return 0;/' $(PROJECT_BUILD)/Src/main.c $(HIDE)
	$(MUTE) sed -i 's/HAL_GPIO_EXTI_IRQHandler/HAL_GPIO_EXTI_Callback/' $(ITC) $(HIDE)
	$(MUTE) sed -i 's/USE_HAL_UART_REGISTER_CALLBACKS\s*0U/USE_HAL_UART_REGISTER_CALLBACKS\t1U/g' $(HALCONF) $(HIDE)
	$(MUTE) $(call copy_file, $(PROJECT_BUILD)/Src/main.c, $(PROJECT_BUILD)/Src/generated_main.c) $(HIDE)
	$(MUTE) $(call remove_file, $(PROJECT_BUILD)/Src/main.c) $(HIDE)

	$(MUTE) $(call remove_file, $(PROJECT_BUILD)/Src/syscalls.c) $(HIDE)

	$(MUTE) $(foreach inc, $(EXTRA_INC_PATHS), sed -i '/Core\/Inc"\/>/a <listOptionValue builtIn="false" value="$(inc)"\/>' $(PROJECT_BUILDROOT)/.cproject;) $(HIDE)
	$(MUTE) $(foreach flag, $(CPROJECTFLAGS), sed -i '/USE_HAL_DRIVER"\/>/a <listOptionValue builtIn="false" value="$(flag)"\/>' $(PROJECT_BUILDROOT)/.cproject;) $(HIDE)
	$(MUTE) $(STM32CUBEIDE)/$(IDE) -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
		-import $(PROJECT_BUILDROOT) -data $(BUILD_DIR) \
		$(HIDE)

$(PLATFORM)_sdkopen:
	$(STM32CUBEIDE)/$(IDE) -nosplash -import $(PROJECT_BUILDROOT) -data $(BUILD_DIR) &

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

.PHONY: $(BINARY).gdb
$(BINARY).gdb:
	@echo target remote localhost:3333 > $(BINARY).gdb
	@echo load $(BINARY) >> $(BINARY).gdb
	@echo file $(BINARY) >> $(BINARY).gdb
	@echo monitor reset halt >> $(BINARY).gdb
	@echo tb main >> $(BINARY).gdb
	@echo tb HardFault_Handler >> $(BINARY).gdb
	@echo tui enable >> $(BINARY).gdb
	@echo c >> $(BINARY).gdb

HEX = $(basename $(BINARY)).hex

$(HEX): $(BINARY)
	$(MUTE) $(call print,[HEX] $(notdir $@))
	$(MUTE) $(OC) -O ihex $(BINARY) $(HEX)
	$(MUTE) $(call print,$(notdir $@) is ready)

post_build: $(HEX)

PHONY += $(PLATFORM)_sdkbuild
$(PLATFORM)_sdkbuild:
	$(MUTE) $(STM32CUBEIDE)/$(IDE) -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
		-import $(PROJECT_BUILDROOT) -data $(BUILD_DIR) -build app $(HIDE)

PHONY += $(PLATFORM)_sdkclean
$(PLATFORM)_sdkclean:
	$(call print,[Delete] SDK artefacts from $(DEBUG_DIR))
	$(MUTE) $(call remove_dir,$(DEBUG_DIR)) $(HIDE)
	$(call print,[Delete] SDK artefacts from $(RELEASE_DIR))
	$(MUTE) $(call remove_dir,$(RELEASE_DIR)) $(HIDE)

clean_hex:
	@$(call print,[Delete] $(HEX))
	$(MUTE) $(call remove_file,$(HEX)) $(HIDE)

clean: clean_hex

.PHONY: stm32_run
$(PLATFORM)_run: all $(BINARY).openocd
	$(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" -f $(BINARY).openocd \
		-c "program $(BINARY) verify reset exit"

.PHONY: debug
debug: all $(BINARY).openocd $(BINARY).gdb
	($(OPENOCD_BIN)/openocd -s "$(OPENOCD_SCRIPTS)" -f $(BINARY).openocd \
		-c "init" &);
	$(GDB) --command=$(BINARY).gdb

