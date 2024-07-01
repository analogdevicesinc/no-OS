VSCODE_SUPPORT = yes

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

# Locate the compiler path under STM32CubeIDE plugins directory
COMPILER_BIN = $(realpath $(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *arm-none-eabi-gcc)))
COMPILER_INTELLISENSE_PATH = $(COMPILER_BIN)/arm-none-eabi-gcc

# Locate openocd location under STM32CubeIDE plugins directory
OPENOCD_SCRIPTS = $(realpath $(addsuffix ..,$(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *st_scripts/interface/stlink-dap.cfg))))
OPENOCD_BIN = $(realpath $(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *bin/openocd)))
OPENOCD_SVD = $(word 1,$(realpath $(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *.svd))))
VSCODE_CMSISCFG_FILE = "$(BINARY).openocd-cmsis"
TARGET_HARDWARE=HARDWARE=$(HARDWARE)

# stm32 specific build directory tree (project goes under app, but user .c/.h sources must go under app/Core)
PROJECT_BUILDROOT = $(BUILD_DIR)/app
PROJECT_BUILD = $(PROJECT_BUILDROOT)/Core
RELEASE_DIR = $(PROJECT_BUILDROOT)/Release
DEBUG_DIR = $(PROJECT_BUILDROOT)/Debug
EXTI_GEN_FILE = $(NO-OS)/drivers/platform/stm32/stm32_gpio_irq_generated.c

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
TARGET = $(shell sed -rn 's|^.*(STM32[A-Z][0-9][0-9A-Z][0-9]x.)"/>$$|\1|p' $(PROJECT_BUILDROOT)/.cproject | head -n 1)
CFLAGS += -D$(TARGET)
CHIPNAME = $(shell sed -rn 's|^.*(STM32[A-Z][0-9][0-9A-Z][0-9][A-Z][A-Z][A-Z]x+)" .*$$|\1|p' $(PROJECT_BUILDROOT)/.cproject | head -n 1)
TARGETCFG = $(shell sed -rn "s|^.*(STM32[A-Z][0-9]).*$$|target/\L\1x.cfg|p" $(PROJECT_BUILDROOT)/.cproject | head -n 1)
TARGETSVD = $(shell sed -rn "s|^.*(STM32[A-Z][0-9][0-9A-Z][0-9]).*$$|\1|p" $(PROJECT_BUILDROOT)/.cproject | head -n 1)
endif

# Get the path of the linker script
LSCRIPT=$(wildcard $(PROJECT_BUILDROOT)/*FLASH.ld)

# Get the extra flags that need to be added into the .cproject file
CPROJECTFLAGS = $(sort $(subst -D,,$(filter -D%, $(CFLAGS))))

ifeq ($(NO_OS_USB_UART),y)
SRC_DIRS += $(BUILD_DIR)/app/USB_DEVICE \
	    $(BUILD_DIR)/app/Middlewares/ST/STM32_USB_Device_Library
SRCS += $(NO-OS)/drivers/platform/stm32/stm32_usb_uart.c
INCS += $(NO-OS)/drivers/platform/stm32/stm32_usb_uart.h
endif

$(PLATFORM)_project:
	$(call print,Creating IDE project)
	$(call mk_dir, $(BUILD_DIR))
	$(call mk_dir, $(VSCODE_CFG_DIR))
	@echo config load $(HARDWARE) > $(BINARY).cubemx
	@echo project name app >> $(BINARY).cubemx
	@echo project toolchain STM32CubeIDE >> $(BINARY).cubemx
	@echo project path $(BUILD_DIR) >> $(BINARY).cubemx
	@echo SetCopyLibrary "copy all" >> $(BINARY).cubemx
	@echo SetStructure Advanced >> $(BINARY).cubemx
	@echo project generate >> $(BINARY).cubemx
	@echo exit >> $(BINARY).cubemx
	java -jar $(STM32CUBEMX)/$(MX) -q $(BINARY).cubemx $(HIDE)
	$(call remove_file,$(BINARY).cubemx) $(HIDE)
	$(MAKE) --no-print-directory $(PROJECT)_configure

$(PROJECT)_configure:
	$(call print,Configuring project)
	sed -i 's/ main(/ stm32_init(/' $(PROJECT_BUILD)/Src/main.c $(HIDE)
	sed -i '0,/while (1)/s//return 0;/' $(PROJECT_BUILD)/Src/main.c $(HIDE)
	sed -i 's/USE_HAL_TIM_REGISTER_CALLBACKS\s*0U/USE_HAL_TIM_REGISTER_CALLBACKS\t1U/g' $(HALCONF) $(HIDE)
	sed -i 's/USE_HAL_UART_REGISTER_CALLBACKS\s*0U/USE_HAL_UART_REGISTER_CALLBACKS\t1U/g' $(HALCONF) $(HIDE)
	sed -i 's/USE_HAL_SAI_REGISTER_CALLBACKS\s*0U/USE_HAL_SAI_REGISTER_CALLBACKS\t1U/g' $(HALCONF) $(HIDE)
ifeq ($(NO_OS_USB_UART),y)
	sed -i 's/USBD_Interface_fops_FS/_unused_USBD_Interface_fops_FS/g'  $(BUILD_DIR)/app/USB_DEVICE/App/usbd_cdc_if.c $(HIDE)
endif
	$(call copy_file, $(PROJECT_BUILD)/Src/main.c, $(PROJECT_BUILD)/Src/generated_main.c) $(HIDE)
	$(call remove_file, $(PROJECT_BUILD)/Src/main.c) $(HIDE)

	$(call remove_file, $(PROJECT_BUILD)/Src/syscalls.c) $(HIDE)

	$(foreach inc, $(EXTRA_INC_PATHS), sed -i '/Core\/Inc"\/>/a <listOptionValue builtIn="false" value="$(inc)"\/>' $(PROJECT_BUILDROOT)/.cproject;) $(HIDE)
	$(foreach flag, $(CPROJECTFLAGS), sed -i '/USE_HAL_DRIVER"\/>/a <listOptionValue builtIn="false" value="$(flag)"\/>' $(PROJECT_BUILDROOT)/.cproject;) $(HIDE)
	$(STM32CUBEIDE)/$(IDE) -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
		-import $(PROJECT_BUILDROOT) -data $(BUILD_DIR) \
		$(HIDE)
	sed -i  's/HAL_NVIC_EnableIRQ(\EXTI/\/\/ HAL_NVIC_EnableIRQ\(EXTI/' $(PROJECT_BUILD)/Src/generated_main.c $(HIDE)
	$(shell python $(PLATFORM_TOOLS)/exti_script.py $(ASM_SRCS) $(EXTI_GEN_FILE))
	$(call copy_file, $(EXTI_GEN_FILE), $(PROJECT_BUILD)/Src/stm32_gpio_irq_generated.c) $(HIDE)

	$(file > $(CPP_PROP_JSON).default,$(CPP_FINAL_CONTENT))
	$(file > $(SETTINGSJSON).default,$(VSC_SET_CONTENT))
	$(file > $(LAUNCHJSON).default,$(VSC_LAUNCH_CONTENT))
	$(file > $(TASKSJSON).default,$(VSC_TASKS_CONTENT))

	[ -s $(CPP_PROP_JSON) ]	&& echo '.vscode/c_cpp_properties.json already exists, not overwriting'	|| cp $(CPP_PROP_JSON).default $(CPP_PROP_JSON)
	[ -s $(SETTINGSJSON) ] 	&& echo '.vscode/settings.json already exists, not overwriting'			|| cp $(SETTINGSJSON).default $(SETTINGSJSON)
	[ -s $(LAUNCHJSON) ] 	&& echo '.vscode/launch.json already exists, not overwriting'			|| cp $(LAUNCHJSON).default $(LAUNCHJSON)
	[ -s $(TASKSJSON) ] 	&& echo '.vscode/tasks.json already exists, not overwriting'			|| cp $(TASKSJSON).default $(TASKSJSON)

	rm $(CPP_PROP_JSON).default $(SETTINGSJSON).default $(LAUNCHJSON).default $(TASKSJSON).default

	$(MAKE) $(BINARY).openocd-cmsis
	$(MAKE) $(BINARY).openocd

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
	-mfloat-abi=$(CFLAGS_MFLOAT_TYPE) \
	-mfpu=fpv4-sp-d16 \
	-mcpu=cortex-m4

LDFLAGS	= -mcpu=cortex-m4 \
	-Wl,--gc-sections \
	-static \
	--specs=nosys.specs \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=$(CFLAGS_MFLOAT_TYPE) \
	-mthumb \
	-Wl,--start-group \
	-lc \
	-lm \
	-Wl,--end-group

AR = arm-none-eabi-ar
AS = arm-none-eabi-gcc
CC = arm-none-eabi-gcc
GDB = arm-none-eabi-gdb
GDB_PORT = 50000
OC = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

ifeq (,$(COMPILER_BIN))
COMPILER_BIN = $(realpath $(dir $(call rwildcard, $(STM32CUBEIDE)/plugins, *arm-none-eabi-gcc)))
endif
export PATH := $(COMPILER_BIN):$(PATH)

.PHONY: $(BINARY).openocd-cmsis
$(BINARY).openocd-cmsis:
	@echo source [find interface/cmsis-dap.cfg] > $(BINARY).openocd-cmsis
	@echo transport select swd >> $(BINARY).openocd-cmsis
	@echo gdb_port $(GDB_PORT) >> $(BINARY).openocd-cmsis
	@echo reset_config srst_only >> $(BINARY).openocd-cmsis
	@echo source [find $(TARGETCFG)] >> $(BINARY).openocd-cmsis

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
	@echo gdb_port $(GDB_PORT) >> $(BINARY).openocd
	@echo source [find $(TARGETCFG)] >> $(BINARY).openocd

.PHONY: $(BINARY).gdb
$(BINARY).gdb:
	@echo target remote localhost:$(GDB_PORT) > $(BINARY).gdb
	@echo load $(BINARY) >> $(BINARY).gdb
	@echo file $(BINARY) >> $(BINARY).gdb
	@echo monitor reset halt >> $(BINARY).gdb
	@echo tb main >> $(BINARY).gdb
	@echo tb HardFault_Handler >> $(BINARY).gdb
	@echo tui enable >> $(BINARY).gdb
	@echo c >> $(BINARY).gdb

HEX = $(basename $(BINARY)).hex

$(HEX): $(BINARY)
	$(call print,[HEX] $(notdir $@))
	$(OC) -O ihex $(BINARY) $(HEX)
	$(call print,$(notdir $@) is ready)

$(PLATFORM)_post_build: $(HEX)

PHONY += $(PLATFORM)_sdkbuild
$(PLATFORM)_sdkbuild:
	$(STM32CUBEIDE)/$(IDE) -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
		-import $(PROJECT_BUILDROOT) -data $(BUILD_DIR) -build app $(HIDE)

PHONY += $(PLATFORM)_sdkclean
$(PLATFORM)_sdkclean:
	$(call print,[Delete] SDK artefacts from $(DEBUG_DIR))
	$(call remove_dir,$(DEBUG_DIR)) $(HIDE)
	$(call print,[Delete] SDK artefacts from $(RELEASE_DIR))
	$(call remove_dir,$(RELEASE_DIR)) $(HIDE)

clean_hex:
	@$(call print,[Delete] $(HEX))
	$(call remove_file,$(HEX)) $(HIDE)

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

$(PLATFORM)_reset:
	$(call remove_dir,$(VSCODE_CFG_DIR))
