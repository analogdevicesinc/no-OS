################################################################################
# Lattice RISC-V RX platform integration                                       #
################################################################################

# Tested with Lattice Propel SDK version: 2025.2 ###############################

# Tested with RISCV-RX core; VLNV: latticesemi.com:ip:riscv_rtos:2.8.0 #########
# Supported RISC-V extentions: M, A ############################################

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
LATTICE_RISCV_ARCH  ?= rv32ima
LATTICE_PROPEL_VERSION ?= 2025.2
LATTICE_RISCV_RTOS_VERSION ?= 2.8.0
LATTICE_IGNORE_ARCH_CHECK ?= n
LATTICE_IGNORE_VERSION_CHECK ?= n
LATTICE_IGNORE_CPU_VERSION_CHECK ?= n

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
#          Hardware SGE setup: extraction/copying + BSP source discovery       #
################################################################################

# HARDWARE parameter can contain:
#  * sge.zip     - Lattice Propel SDK archive (extracted to $(PROJECT)/sge)
#  * /path/to/sge - External SGE directory (copied to $(PROJECT)/sge)
# The canonical runtime location for all SGE files is always LATTICE_SGE_DIR.
# BSP sources from LATTICE_SGE_DIR/bsp are automatically appended to SRC_DIRS
# so generic.mk discovers all source/header files beneath them.
LATTICE_PROJECT_SGE_DIR ?= $(PROJECT)/sge
LATTICE_SGE_ZIP ?= $(lastword $(filter %sge.zip,$(HARDWARE)))
LATTICE_EXTERNAL_SGE_DIR ?= $(lastword $(filter %sge %sge/,$(HARDWARE)))

# If either SGE source is provided, canonicalize to local project directory.
# Otherwise default to local project directory.
ifneq ($(strip $(LATTICE_SGE_ZIP) $(LATTICE_EXTERNAL_SGE_DIR)),)
LATTICE_SGE_DIR := $(LATTICE_PROJECT_SGE_DIR)
else
LATTICE_SGE_DIR ?= $(LATTICE_PROJECT_SGE_DIR)
endif

LATTICE_BSP_PATH ?= $(LATTICE_SGE_DIR)/bsp

# Append LATTICE BSP to build sources if the directory exists.
ifneq (,$(wildcard $(LATTICE_BSP_PATH)))
SRC_DIRS += $(LATTICE_BSP_PATH)
endif

ifneq ($(strip $(LATTICE_OPENOCD_CPU_YAML)),)
ifneq (,$(wildcard $(LATTICE_OPENOCD_CPU_YAML)))
LATTICE_OPENOCD_CPU_CMD := -c "set RISCV_SMALL_YAML {$(LATTICE_OPENOCD_CPU_YAML)}"
endif
endif

################################################################################
#                    sys_env parser configuration                              #
################################################################################

# Input sys_env.xml path (can be overridden by user).
LATTICE_SYS_ENV ?= $(LATTICE_SGE_DIR)/sys_env.xml

# Generated make fragment containing parsed values (versions, JTAG channel, etc.).
LATTICE_SYS_ENV_VARS := $(BUILD_DIR)/sys_env_parsed.tmp

# Resolve parser script path relative to generic.mk location.
LATTICE_MK_FILE := $(lastword $(filter %generic.mk,$(MAKEFILE_LIST)))
ifeq ($(strip $(LATTICE_MK_FILE)),)
$(error [lattice] generic.mk not found in MAKEFILE_LIST; include tools/scripts/generic.mk before lattice.mk)
endif
LATTICE_MK_DIR := $(dir $(LATTICE_MK_FILE))

# Use make-based sys_env parser fragment.
include $(LATTICE_MK_DIR)/platform/lattice/sys_env_parse.mk

################################################################################
#                          Convenience platform targets                        #
################################################################################

PHONY += lattice_reset
lattice_reset:
	@true

PHONY += reset-all
reset-all: reset
	$(call print,[Delete] $(LATTICE_PROJECT_SGE_DIR))
	$(call remove_dir,$(LATTICE_PROJECT_SGE_DIR))

$(LATTICE_SGE_DIR):
ifneq (,$(wildcard $(LATTICE_SGE_ZIP)))
	$(call print,[lattice] Extracting $(LATTICE_SGE_ZIP) into $(PROJECT))
	@unzip -oq "$(LATTICE_SGE_ZIP)" -d "$(PROJECT)"
else ifneq ($(strip $(LATTICE_EXTERNAL_SGE_DIR)),)
ifneq ($(abspath $(LATTICE_EXTERNAL_SGE_DIR)),$(abspath $(LATTICE_PROJECT_SGE_DIR)))
	$(call print,[lattice] Copying $(LATTICE_EXTERNAL_SGE_DIR) into $(LATTICE_PROJECT_SGE_DIR))
	@cp -r "$(LATTICE_EXTERNAL_SGE_DIR)" "$(LATTICE_PROJECT_SGE_DIR)"
endif
endif
	@test -d "$@"

$(LATTICE_BSP_PATH): $(LATTICE_SGE_DIR)
	@test -d "$@"

$(LATTICE_SYS_ENV): $(LATTICE_SGE_DIR)
	@test -f "$@"

# Bitstream programming support (template.xcf flow).
include $(LATTICE_MK_DIR)/platform/lattice/bitstream.mk

# Reuse an existing ELF for `make run`; otherwise fall back to `all` so the
# full build flow runs when no binary has been produced yet.
ifneq ($(wildcard $(BINARY)),)
$(info $(shell $(call print, [Using existing binary: $(BINARY)])))
LATTICE_RUN_DEP := $(BINARY)
else
LATTICE_RUN_DEP := all
endif

# Optional hook for programming a bitstream before run.
# Default is enabled; set BITSTREAM=n to disable.
BITSTREAM ?= y
ifneq (,$(filter y Y yes YES 1 true TRUE,$(strip $(BITSTREAM))))
ifneq ($(strip $(LATTICE_BIT)),)
LATTICE_RUN_DEP += program
else
$(info $(shell $(call print, WARNING: BITSTREAM=y but no .bit file found)))
endif
endif

PHONY += lattice_run
lattice_run: $(LATTICE_RUN_DEP) $(LATTICE_SYS_ENV_VARS)
	$(call print,[lattice] [Uploading $(BINARY) via OpenOCD])
	@. "$(LATTICE_SYS_ENV_VARS)"; \
	channel=$${JTAG_CHANNEL:-$(LATTICE_OPENOCD_CHANNEL)}; \
	$(LATTICE_OPENOCD) $(LATTICE_OPENOCD_SCRIPT_OPT) \
		-c "echo \"DEBUG_ENABLE=$(LATTICE_OPENOCD_DEBUG_ENABLE)\"" \
		-c "set target $(LATTICE_OPENOCD_DEVICE)" \
		-c "set tck $(LATTICE_OPENOCD_TCKDIV)" \
		-c "set port $(LATTICE_OPENOCD_PORT)" \
		-c "set channel $$channel" \
		-c "set cmdlength $(LATTICE_OPENOCD_CMD_LENGTH)" \
		-c "set loc $(LATTICE_OPENOCD_LOCATION)" \
		$(LATTICE_OPENOCD_CPU_CMD) \
		$(LATTICE_OPENOCD_INTERFACE_OPT) $(LATTICE_OPENOCD_TARGET_OPT) \
		$(LATTICE_OPENOCD_EXTRA_ARGS) $(LATTICE_OPENOCD_PROGRAM_OPT)

PHONY += lattice_sdkbuild lattice_sdkclean lattice_sdkopen
lattice_sdkbuild lattice_sdkclean lattice_sdkopen:
	@true


$(LATTICE_LST): $(BINARY)
	$(call print,[Creating $(LATTICE_LST) ...])
	$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide $(BINARY) > $(LATTICE_LST)
	$(call print,[$(LATTICE_LST) created])

$(LATTICE_SIZ): $(BINARY)
	$(call print,[Creating $(LATTICE_SIZ) ...])
	$(SIZE) --format=berkeley $(BINARY) > $(LATTICE_SIZ)
	$(call print,[$(LATTICE_SIZ) created])

$(LATTICE_BIN): $(BINARY)
	$(call print,[Creating $(LATTICE_BIN) ...])
	$(OC) -O binary --gap-fill 0 $(BINARY) $(LATTICE_BIN)
	$(call print,[$(LATTICE_BIN) created])

$(LATTICE_MEM): $(LATTICE_BIN)
	$(call print,[Creating $(LATTICE_MEM) ...])
	$(SREC_CAT) $(LATTICE_BIN) -Binary -byte-swap 4 -DISable Header -Output $(LATTICE_MEM) -MEM 32
	$(call print,[$(LATTICE_MEM) created ])

PHONY += lattice_post_build
lattice_post_build: $(LATTICE_LST) $(LATTICE_SIZ) $(LATTICE_BIN) $(LATTICE_MEM)

PHONY += lattice_project
lattice_project: $(LATTICE_BSP_PATH) sys-env sys-env-check
	$(call mk_dir,$(BUILD_DIR))
