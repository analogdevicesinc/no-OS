#------------------------------------------------------------------------------
#                           ENVIRONMENT VARIABLES                              
#------------------------------------------------------------------------------

TEMP_DIR	= $(BUILD_DIR)/tmp
BINARY		= $(BUILD_DIR)/$(PROJECT_NAME).elf
BOOT_BIN_DIR    = $(BUILD_DIR)/output_boot_bin
FSBL_PATH	= $(TEMP_DIR)/output/hw0/export/hw0/sw/hw0/boot/fsbl.elf
TEMP_DIR_RUN = $(PROJECT)/tmp
FSBL_PATH_RUN	= $(TEMP_DIR_RUN)/output/hw0/export/hw0/sw/hw0/boot/fsbl.elf
FILE := $(wildcard *.elf)
comma	:= ,

ifeq (y,$(strip $(NETWORKING)))
TEMPLATE	= "lwIP Echo Server"
else
TEMPLATE	= "Empty Application(C)"
endif

define tcl_util
	xsct -nodisp $(PLATFORM_TOOLS)/util.tcl					\
	     $(1)							\
	     $(WORKSPACE) $(WORKSPACE)/tmp $(notdir $(HARDWARE))	\
	     $(BINARY) $(TARGET_CPU) $(TEMPLATE)
endef

ARCH = $(shell $(call read_file, $(TEMP_DIR)/arch.txt))
ARCH_RUN = $(shell $(call read_file, $(TEMP_DIR_RUN)/arch.txt))

define create_bif_file
	@echo the_ROM_image: > $(BOOT_BIN_DIR)/project.bif
	@echo { >> $(BOOT_BIN_DIR)/project.bif
	@echo $1 $(FSBL_PATH) >> $(BOOT_BIN_DIR)/project.bif
	@echo $2 $(TEMP_DIR)/*.bit >> $(BOOT_BIN_DIR)/project.bif
	@echo $3 $(BINARY) >> $(BOOT_BIN_DIR)/project.bif
	@echo } >> $(BOOT_BIN_DIR)/project.bif
	@echo >> $(BOOT_BIN_DIR)/project.bif
endef

# Define the platform compiler switch
CFLAGS += -DXILINX_PLATFORM						\
	 -fdata-sections						\
	 -ffunction-sections 						\
	 -O2								\
	 -g3
# Xilinx's generated linker script path
LSCRIPT		:= $(BUILD_DIR)/app/src/lscript.ld
# Xilinx's generate bsp library path
LIB_PATHS	+= -L$(BUILD_DIR)/bsp/$(ARCH)/lib

JTAG_CABLE_ID = $2
TARGET_CPU ?= 0
PROJECT_BUILD = $(BUILD_DIR)/app

################|--------------------------------------------------------------
################|                   Zynq                                       
################|--------------------------------------------------------------
ifneq (,$(findstring cortexa9,$(strip $(ARCH))))

CC := arm-none-eabi-gcc
AR := arm-none-eabi-ar
SIZE := arm-none-eabi-size

LD := $(CC)

CFLAGS += -mcpu=cortex-a9 						\
	  -mfpu=vfpv3 							\
	  -mfloat-abi=$(CFLAGS_MFLOAT_TYPE)

LDFLAGS += -specs=$(BUILD_DIR)/app/src/Xilinx.spec 			\
	   -mfpu=vfpv3							\
 	   -mfloat-abi=$(CFLAGS_MFLOAT_TYPE)				\
	   -mcpu=cortex-a9						\
	   -Wl,-build-id=none

endif

################|--------------------------------------------------------------
################|                   ZynqMP                                     
################|--------------------------------------------------------------
ifneq (,$(findstring cortexa53,$(strip $(ARCH))))

CC := aarch64-none-elf-gcc
AR := aarch64-none-elf-ar
SIZE := aarch64-none-elf-size

LD := $(CC)
endif

ifneq (,$(findstring cortexr5,$(strip $(ARCH))))

CC := armr5-none-eabi-gcc 
AR := armr5-none-eabi-ar
SIZE := armr5-none-eabi-size

LD := $(CC)

CFLAGS += -mcpu=cortex-r5						\
	  -mfloat-abi=$(CFLAGS_MFLOAT_TYPE)				\
	  -mfpu=vfpv3-d16

LDFLAGS += -mcpu=cortex-r5						\
           -mfloat-abi=$(CFLAGS_MFLOAT_TYPE)				\
	   -mfpu=vfpv3-d16						\
	   -DARMR5
endif

################|--------------------------------------------------------------
################|                   Versal                                     
################|--------------------------------------------------------------
ifneq (,$(findstring cortexa72,$(strip $(ARCH))))

CC := aarch64-none-elf-gcc
AR := aarch64-none-elf-ar
SIZE := aarch64-none-elf-size

LD := $(CC)
endif

################|--------------------------------------------------------------
################|                  Microblaze                                  
################|--------------------------------------------------------------
ifneq (,$(findstring sys_mb,$(strip $(ARCH))))

ifeq ($(OS), Windows_NT)
CC := mb-gcc
AR := mb-ar
SIZE := mb-size
else
CC := microblaze-xilinx-elf-gcc
AR := microblaze-xilinx-elf-ar
SIZE := microblaze-xilinx-elf-size
endif

LD := $(CC)

CFLAGS += -mcpu=cortex-a9 						\
	  -DXILINX -DMICROBLAZE						\
	  -DXILINX_PLATFORM						\
	  -mlittle-endian						\
	  -mxl-barrel-shift						\
	  -mxl-pattern-compare 						\
	  -mno-xl-soft-div						\
	  -mcpu=v11.0							\
	  -mno-xl-soft-mul						\
	  -mxl-multiply-high

LDFLAGS += -Xlinker --defsym=_HEAP_SIZE=0x100000 			\
	   -Xlinker --defsym=_STACK_SIZE=0x2000 			\
	   -mlittle-endian 						\
	   -mxl-barrel-shift						\
	   -mxl-pattern-compare						\
	   -mno-xl-soft-div						\
	   -mcpu=v11.0							\
	   -mno-xl-soft-mul						\
	   -mxl-multiply-high 						\
	   -Wl,--no-relax 						\
	   -Wl,--gc-sections 

endif

# Common xilinx libs
LIB_FLAGS += -Wl,--start-group,-lxil,-lgcc,-lc,--end-group

# Add the common include paths
CFLAGS += -I$(BUILD_DIR)/app/src
# Xilinx's bsp include path
CFLAGS		+= -I$(BUILD_DIR)/bsp/$(ARCH)/include

$(PLATFORM)_sdkopen:
ifeq '' '$(filter %.hdf, $(HARDWARE))'
	vitis -workspace=$(WORKSPACE)
else
	xsdk -workspace=$(WORKSPACE)
endif

#Add more dependencies to $(BINARY) rule.
$(BINARY): $(TEMP_DIR)/arch.txt

xilinx_run:
ifneq (, $(wildcard *.elf))
	$(call print,Elf exists $(FILE))
	$(MAKE) --no-print-directory $(TEMP_DIR_RUN)
	xsct -nodisp $(PLATFORM_TOOLS)/util.tcl					\
	     upload							\
	     $(PROJECT) $(TEMP_DIR_RUN) $(notdir $(HARDWARE))	\
	     $(PROJECT)/$(FILE) $(TARGET_CPU) $(TEMPLATE)       \
		 $(FSBL_PATH_RUN) $(JTAG_CABLE_ID) $(HIDE)
else
ifneq (, $(wildcard build))
	$(call print, build folder exists)
	$(call tcl_util, upload) $(FSBL_PATH) $(JTAG_CABLE_ID) $(HIDE)
else
	$(call print,Can not perform make run command)
endif
endif

$(TEMP_DIR_RUN): $(HARDWARE)
	$(call print,Creating tmp directory)
	$(call mk_dir,$(TEMP_DIR_RUN)) $(HIDE)
	$(call copy_file,$(HARDWARE),$(TEMP_DIR_RUN)) $(HIDE)
	xsct -nodisp $(PLATFORM_TOOLS)/util.tcl					\
	     get_arch						\
	     $(PROJECT) $(TEMP_DIR_RUN) $(notdir $(HARDWARE))	\
	     $(PROJECT)/$(FILE) $(TARGET_CPU) $(TEMPLATE) $(HIDE)
	$(MAKE) --no-print-directory create_fsbl

PHONY += create_fsbl
create_fsbl:
ifeq ($(findstring cortexa53,$(strip $(ARCH_RUN))),cortexa53)
	$(call print,genera $(ARCH_RUN) fsbl)
	$(call copy_file,$(NO-OS)/tools/scripts/platform/xilinx/create_fsbl_project.tcl,$(TEMP_DIR_RUN)) $(HIDE)
	xsct -nodisp $(TEMP_DIR_RUN)/create_fsbl_project.tcl $(PROJECT) $(TEMP_DIR_RUN)/$(notdir $(HARDWARE)) $(HIDE)
endif
ifeq ($(findstring cortexr5,$(strip $(ARCH_RUN))),cortexr5)
	$(call print,genera $(ARCH_RUN) fsbl)
	$(call copy_file,$(NO-OS)/tools/scripts/platform/xilinx/create_fsbl_project.tcl,$(TEMP_DIR_RUN)) $(HIDE)
	xsct -nodisp $(TEMP_DIR_RUN)/create_fsbl_project.tcl $(PROJECT) $(TEMP_DIR_RUN)/$(notdir $(HARDWARE)) $(HIDE)
endif

$(TEMP_DIR)/arch.txt: $(HARDWARE)
	$(call mk_dir,$(BUILD_DIR)/app $(BUILD_DIR)/app/src $(OBJECTS_DIR) $(TEMP_DIR)) $(HIDE)
	$(call copy_file,$(HARDWARE),$(TEMP_DIR)) $(HIDE)
	$(call print,Evaluating hardware: $(HARDWARE))
	$(call tcl_util, get_arch) $(HIDE)

PHONY += $(PLATFORM)_sdkbuild
$(PLATFORM)_sdkbuild:
	xsct -nodisp $(NO-OS)/tools/scripts/platform/xilinx/build_project.tcl $(WORKSPACE) $(HIDE)

PHONY += $(PLATFORM)_sdkclean
$(PLATFORM)_sdkclean:
	$(call print,[Delete] SDK artefacts from $(BUILD_DIR))
	$(call tcl_util, clean_build) $(HIDE)

$(PLATFORM)_project: $(TEMP_DIR)/arch.txt
	$(call print,Creating and configuring the IDE project)
	$(call tcl_util, create_project)  $(HIDE)
ifeq (y,$(strip $(NETWORKING)))
	$(call remove_file,$(BUILD_DIR)/app/src/main.c $(BUILD_DIR)/app/src/echo.c) $(HIDE)
endif
	$(call print,Creating fsbl.elf)
	$(call copy_file,$(NO-OS)/tools/scripts/platform/xilinx/create_fsbl_project.tcl,$(TEMP_DIR)) $(HIDE)
	xsct -nodisp $(TEMP_DIR)/create_fsbl_project.tcl $(BUILD_DIR) $(TEMP_DIR)/$(notdir $(HARDWARE)) $(HIDE)

PHONY += create_boot_bin
create_boot_bin:
ifneq ($(findstring cortexa72,$(strip $(ARCH))),cortexa72)
ifneq ($(findstring sys_mb,$(strip $(ARCH))),sys_mb)
	$(call print,Creating BOOT.BIN and archive with files)
	$(call remove_dir,$(BOOT_BIN_DIR)) $(HIDE)
	$(call mk_dir,$(BOOT_BIN_DIR)) $(HIDE)
ifeq ($(findstring cortexa53,$(strip $(ARCH))),cortexa53)
	$(call create_bif_file,[bootloader$(comma)destination_cpu = a53-0],[destination_device = pl],[destination_cpu = a53-0]) $(HIDE)
	bootgen -arch zynqmp -image $(BOOT_BIN_DIR)/project.bif -o $(BOOT_BIN_DIR)/BOOT.BIN -w $(HIDE)
endif
ifeq ($(findstring cortexa9,$(strip $(ARCH))),cortexa9)
	$(call create_bif_file,[bootloader],,) $(HIDE)
	bootgen -arch zynq -image $(BOOT_BIN_DIR)/project.bif -o $(BOOT_BIN_DIR)/BOOT.BIN -w $(HIDE)
endif
ifeq ($(findstring cortexr5,$(strip $(ARCH))),cortexr5)
	$(call create_bif_file,[bootloader$(comma)destination_cpu = r5-0],[destination_device = pl],[destinatio_cpu = r5-0]) $(HIDE)
	bootgen -arch zynqmp -image $(BOOT_BIN_DIR)/project.bif -o $(BOOT_BIN_DIR)/BOOT.BIN -w $(HIDE)
endif
	$(call copy_file,$(HARDWARE),$(BOOT_BIN_DIR)) $(HIDE)
	$(call copy_file,$(FSBL_PATH),$(BOOT_BIN_DIR)) $(HIDE)
	$(call copy_file,$(BINARY),$(BOOT_BIN_DIR)) $(HIDE)
	tar -czvf $(BUILD_DIR)/bootgen_sysfiles.tar.gz --transform 's/^\(\.\/\|\.\)//' --force-local --exclude 'BOOT.BIN' -C $(BOOT_BIN_DIR) . $(HIDE)
else
	$(call print,Creating archive with files)
	$(call remove_dir,$(BUILD_DIR)/boot_files) $(HIDE)
	$(call mk_dir,$(BUILD_DIR)/boot_files) $(HIDE)
	$(call copy_file,$(HARDWARE),$(BUILD_DIR)/boot_files) $(HIDE)
	$(call copy_file,$(BINARY),$(BUILD_DIR)/boot_files) $(HIDE)
	tar -czvf $(BUILD_DIR)/bootgen_sysfiles.tar.gz --transform 's/^\(\.\/\|\.\)//' --force-local -C $(BUILD_DIR)/boot_files . $(HIDE)
endif
endif

$(PLATFORM)_post_build:
	$(MAKE) --no-print-directory create_boot_bin

$(PLATFORM)_reset: xilinx_clean_all

xilinx_clean_all:
	$(call remove_dir,.Xil) $(HIDE)
