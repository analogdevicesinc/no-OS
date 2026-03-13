################################################################################
# Lattice RISC-V RX platform integration                                       #
################################################################################

# Tested with Lattice Propel SDK version: 2024.2 ###############################

# Tested with RISCV-RX core; VLNV: latticesemi.com:ip:riscv_rtos:2.5.0 #########
# Supported RISC-V extentions: M, A, C #########################################

# Toolchain prefix (can be overridden by the user)
RISCV_PREFIX          ?= riscv-none-embed-

override AR           = $(RISCV_PREFIX)ar
override AS           = $(RISCV_PREFIX)gcc
override CC           = $(RISCV_PREFIX)gcc
override OC           = $(RISCV_PREFIX)objcopy
override SIZE         = $(RISCV_PREFIX)size
override OBJDUMP      = $(RISCV_PREFIX)objdump
override GDB          = $(RISCV_PREFIX)gdb
SREC_CAT              ?= srec_cat

################################################################################
#                     Optional Propel SDK path helpers                         #
################################################################################

LATTICE_PROPEL_SDK    ?=
LATTICE_RISCV_GCC_BIN ?=
LATTICE_SREC_CAT_BIN  ?=
LATTICE_OPENOCD_BIN   ?=

ifneq ($(strip $(LATTICE_PROPEL_SDK)),)
LATTICE_RISCV_GCC_BIN ?= $(LATTICE_PROPEL_SDK)/sdk/riscv-none-embed-gcc/bin
LATTICE_SREC_CAT_BIN  ?= $(LATTICE_PROPEL_SDK)/sdk/tools/bin
LATTICE_OPENOCD_BIN   ?= $(LATTICE_PROPEL_SDK)/openocd/bin
endif

ifneq ($(strip $(LATTICE_RISCV_GCC_BIN)),)
ifeq (,$(strip $(shell command -v $(RISCV_PREFIX)gcc 2>/dev/null)))
export PATH := $(LATTICE_RISCV_GCC_BIN):$(PATH)
endif
endif

ifneq ($(strip $(LATTICE_SREC_CAT_BIN)),)
ifeq (,$(strip $(shell command -v $(SREC_CAT) 2>/dev/null)))
export PATH := $(LATTICE_SREC_CAT_BIN):$(PATH)
endif
endif

################################################################################
#                      C library / specs selection                             #
################################################################################

LIBC ?= picolibc
ifeq ($(LIBC),newlib)
LIBC_SPECS   := --specs=nosys.specs
LIBC_DEFINES := -DNEWLIB
else ifeq ($(LIBC),newlib-nano)
LIBC_SPECS   := --specs=nano.specs
LIBC_DEFINES := -DNEWLIB_NANO
else ifeq ($(LIBC),picolibc)
LIBC_SPECS   := --specs=picolibc.specs
LIBC_DEFINES := -DPICOLIBC_DOUBLE_PRINTF_SCANF -DPICOLIBC
else
$(error Unsupported LIBC '$(LIBC)'; use newlib, newlib-nano or picolibc)
endif

################################################################################
#                            Compiler / Linker flags                           #
################################################################################
LATTICE_RISCV_ARCH  ?= rv32imac
LATTICE_PROPEL_VERSION ?= 2024.2
LATTICE_RISCV_RTOS_VERSION ?= 2.5.0

LATTICE_RISCV_ABI ?=
ifeq ($(strip $(LATTICE_RISCV_ABI)),)
ifneq ($(findstring d,$(LATTICE_RISCV_ARCH)),)
LATTICE_RISCV_ABI := ilp32d
else ifneq ($(findstring f,$(LATTICE_RISCV_ARCH)),)
LATTICE_RISCV_ABI := ilp32f
else
LATTICE_RISCV_ABI := ilp32
endif
endif

LATTICE_ARCH_FLAGS   ?= -march=$(LATTICE_RISCV_ARCH) -mabi=$(LATTICE_RISCV_ABI) -msmall-data-limit=8 -mno-save-restore
LATTICE_DEBUG        ?= y
LATTICE_OPT_FLAGS    ?= -Os
ifeq (y,$(strip $(LATTICE_DEBUG)))
LATTICE_OPT_FLAGS := -O0 -g3
endif
LATTICE_COMMON_FLAGS ?= $(LATTICE_OPT_FLAGS) -fmessage-length=0 -fsigned-char \
	   -ffunction-sections -fdata-sections
LATTICE_DEFINES      ?= -DLATTICE_PLATFORM
LATTICE_CSTD         ?= -std=gnu11
LATTICE_ASFLAGS      ?= -x assembler-with-cpp
LATTICE_LDFLAGS      ?= -nostartfiles -Xlinker --gc-sections
LATTICE_MAP          ?= $(BUILD_DIR)/$(BINARY_FILE_NAME).map
LATTICE_LST          ?= $(BUILD_DIR)/$(BINARY_FILE_NAME).lst
LATTICE_SIZ          ?= $(BUILD_DIR)/$(BINARY_FILE_NAME).siz
LATTICE_BIN          ?= $(BUILD_DIR)/$(BINARY_FILE_NAME).bin
LATTICE_MEM          ?= $(BUILD_DIR)/$(BINARY_FILE_NAME).mem

################################################################################
#                         OpenOCD programming support                          #
################################################################################

ifneq ($(strip $(LATTICE_OPENOCD_BIN)),)
LATTICE_OPENOCD ?= $(LATTICE_OPENOCD_BIN)/openocd
else ifneq ($(strip $(OPENOCD_BIN)),)
LATTICE_OPENOCD ?= $(OPENOCD_BIN)/openocd
else
LATTICE_OPENOCD ?= $(firstword $(shell command -v openocd 2>/dev/null))
endif
ifeq ($(strip $(LATTICE_OPENOCD)),)
LATTICE_OPENOCD := openocd
endif

LATTICE_OPENOCD_SCRIPT_PATH ?= $(OPENOCD_SCRIPTS)
ifneq ($(strip $(LATTICE_OPENOCD_SCRIPT_PATH)),)
LATTICE_OPENOCD_SCRIPT_OPT := -s "$(LATTICE_OPENOCD_SCRIPT_PATH)"
endif

LATTICE_OPENOCD_INTERFACE ?= interface/lattice-cable.cfg
ifneq ($(strip $(LATTICE_OPENOCD_INTERFACE)),)
LATTICE_OPENOCD_INTERFACE_OPT := -f "$(LATTICE_OPENOCD_INTERFACE)"
endif

LATTICE_OPENOCD_TARGET ?= target/riscv-small.cfg
ifneq ($(strip $(LATTICE_OPENOCD_TARGET)),)
LATTICE_OPENOCD_TARGET_OPT := -f "$(LATTICE_OPENOCD_TARGET)"
endif

LATTICE_OPENOCD_DEBUG_ENABLE ?= True
LATTICE_OPENOCD_DEVICE       ?= 0
LATTICE_OPENOCD_TCKDIV       ?= 1
LATTICE_OPENOCD_PORT         ?= 0
LATTICE_OPENOCD_CHANNEL      ?= 14
LATTICE_OPENOCD_CMD_LENGTH   ?= 0
LATTICE_OPENOCD_LOCATION     ?= 0000
LATTICE_OPENOCD_CPU_YAML     ?= $(LATTICE_SGE_DIR)/cpu0.yaml
LATTICE_OPENOCD_EXTRA_ARGS   ?=
LATTICE_OPENOCD_PROGRAM_CMD  ?= init; halt; load_image {$(BINARY)}; resume; shutdown

ifneq ($(strip $(LATTICE_OPENOCD_PROGRAM_CMD)),)
LATTICE_OPENOCD_PROGRAM_OPT := -c "$(LATTICE_OPENOCD_PROGRAM_CMD)"
endif

# Preload cable configuration variables before sourcing interface scripts
LATTICE_OPENOCD_PRESET_CMDS := \
	-c "echo \"DEBUG_ENABLE=$(LATTICE_OPENOCD_DEBUG_ENABLE)\"" \
	-c "set target $(LATTICE_OPENOCD_DEVICE)" \
	-c "set tck $(LATTICE_OPENOCD_TCKDIV)" \
	-c "set port $(LATTICE_OPENOCD_PORT)" \
	-c "set channel $(LATTICE_OPENOCD_CHANNEL)" \
	-c "set cmdlength $(LATTICE_OPENOCD_CMD_LENGTH)" \
	-c "set loc $(LATTICE_OPENOCD_LOCATION)"

CFLAGS  += $(LATTICE_ARCH_FLAGS) $(LATTICE_COMMON_FLAGS) $(LATTICE_CSTD) \
	   $(LATTICE_DEFINES) $(LIBC_DEFINES) $(LIBC_SPECS)
ASFLAGS += $(LATTICE_ARCH_FLAGS) $(LATTICE_COMMON_FLAGS) $(LATTICE_ASFLAGS) \
	   $(LATTICE_DEFINES) $(LIBC_DEFINES) $(LIBC_SPECS)
LDFLAGS += $(LATTICE_ARCH_FLAGS) $(LATTICE_COMMON_FLAGS) $(LATTICE_LDFLAGS) \
	   -Wl,-Map,$(LATTICE_MAP) $(LIBC_DEFINES) $(LIBC_SPECS)

################################################################################
#                      Linker script (can be overridden)                       #
################################################################################

LATTICE_LINKER_SCRIPT ?= $(PROJECT)/src/platform/$(PLATFORM)/linker.ld
ifneq (,$(wildcard $(LATTICE_LINKER_SCRIPT)))
LSCRIPT ?= $(LATTICE_LINKER_SCRIPT)
endif

################################################################################
#                  Optional BSP sources pulled into the build                  #
################################################################################

# LATTICE_BSP_PATH can be a space-separated list of directories containing BSP
# sources (C/S) and headers generated by Propel. When not provided, fall back to
# $(PROJECT)/sge/bsp if it exists. These directories are appended to SRC_DIRS so
# generic.mk will automatically discover all source / header files beneath them.
LATTICE_BSP_PATH ?= $(PROJECT)/sge/bsp
LATTICE_SGE_ZIP ?= $(lastword $(filter %sge.zip,$(HARDWARE)))
ifneq (,$(wildcard $(LATTICE_SGE_ZIP)))
LATTICE_SGE_DIR ?= $(PROJECT)/sge
else
LATTICE_SGE_DIR ?= $(lastword $(filter %sge,$(HARDWARE)))
endif

ifneq (,$(wildcard $(LATTICE_BSP_PATH)))
BSP_DIRS := $(LATTICE_BSP_PATH)

else ifneq (,$(wildcard $(LATTICE_SGE_DIR)/bsp))
BSP_DIRS := $(LATTICE_SGE_DIR)/bsp

else ifneq (,$(wildcard $(LATTICE_SGE_ZIP)))
$(info [lattice] Extracting $(LATTICE_SGE_ZIP) into $(PROJECT))
$(shell unzip -oq "$(LATTICE_SGE_ZIP)" -d "$(PROJECT)")

ifneq (,$(wildcard $(LATTICE_SGE_DIR)/bsp))
BSP_DIRS := $(LATTICE_SGE_DIR)/bsp
else
$(error [lattice] Extracted archive did not provide $(LATTICE_SGE_DIR)/bsp)
endif

else
$(error [lattice] Missing BSP sources: set LATTICE_SGE_DIR or LATTICE_SGE_ZIP or provide $(LATTICE_SGE_DIR) or $(LATTICE_SGE_ZIP))
endif

ifneq ($(strip $(BSP_DIRS)),)
$(foreach bsp,$(BSP_DIRS),$(if $(wildcard $(bsp)),$(eval SRC_DIRS += $(bsp)),$(warning [lattice] Ignoring missing BSP directory '$(bsp)')))
endif

ifneq ($(strip $(LATTICE_OPENOCD_CPU_YAML)),)
ifneq (,$(wildcard $(LATTICE_OPENOCD_CPU_YAML)))
LATTICE_OPENOCD_CPU_CMD := -c "set RISCV_SMALL_YAML {$(LATTICE_OPENOCD_CPU_YAML)}"
endif
endif

################################################################################
#                      sys_env.xml parsing (versions, JTAG)                    #
################################################################################

LATTICE_SYS_ENV     ?= $(LATTICE_SGE_DIR)/sys_env.xml
LATTICE_MK_FILE       := $(lastword $(filter %generic.mk,$(MAKEFILE_LIST)))
ifeq ($(strip $(LATTICE_MK_FILE)),)
$(error [lattice] generic.mk not found in MAKEFILE_LIST; include tools/scripts/generic.mk before lattice.mk)
endif
LATTICE_MK_DIR        := $(dir $(LATTICE_MK_FILE))
LATTICE_PARSE_SYS_ENV := $(abspath \
	$(LATTICE_MK_DIR)/platform/lattice/parse_sys_env.sh)
LATTICE_PARSE_RAW     := $(shell \
	bash "$(LATTICE_PARSE_SYS_ENV)" "$(LATTICE_SYS_ENV)" \
	"$(LATTICE_RISCV_ARCH)" "$(LATTICE_PROPEL_VERSION)" \
	"$(LATTICE_RISCV_RTOS_VERSION)" || echo __PARSE_FAILED__)

ifneq ($(findstring __PARSE_FAILED__,$(LATTICE_PARSE_RAW)),)
$(error [lattice] Failed to parse $(LATTICE_SYS_ENV); see errors above)
endif

$(foreach kv,$(LATTICE_PARSE_RAW),$(eval $(kv)))

LATTICE_OPENOCD_CHANNEL ?= $(JTAG_CHANNEL)

################################################################################
#                          Convenience platform targets                        #
################################################################################

.PHONY: lattice_run lattice_sdkbuild lattice_sdkclean lattice_sdkopen lattice_post_build
.PHONY: lattice_project

clean: lattice_clean

.PHONY: lattice_clean
lattice_clean:
	$(call print,[lattice] Removing build directory $(BUILD_DIR))
	$(call remove_dir,$(BUILD_DIR))

lattice_run: all
	$(call print,[lattice] Uploading $(notdir $(BINARY)) via OpenOCD)
	$(LATTICE_OPENOCD) $(LATTICE_OPENOCD_SCRIPT_OPT) \
		$(LATTICE_OPENOCD_PRESET_CMDS) $(LATTICE_OPENOCD_CPU_CMD) \
		$(LATTICE_OPENOCD_INTERFACE_OPT) $(LATTICE_OPENOCD_TARGET_OPT) \
		$(LATTICE_OPENOCD_EXTRA_ARGS) $(LATTICE_OPENOCD_PROGRAM_OPT)

lattice_sdkbuild lattice_sdkclean lattice_sdkopen:
	@true

lattice_post_build:
	$(call print,Generating lattice artifacts)
	$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide $(BINARY) > $(LATTICE_LST)
	$(SIZE) --format=berkeley $(BINARY) > $(LATTICE_SIZ)
	$(OC) -O binary --gap-fill 0 $(BINARY) $(LATTICE_BIN)
	$(SREC_CAT) $(LATTICE_BIN) -Binary -byte-swap 4 -DISable Header -Output $(LATTICE_MEM) -MEM 32

lattice_project:
	$(call mk_dir,$(BUILD_DIR))
