export PROCESSOR_CORE 
export HARDWARE
export PLATFORM

NO-OS 		  = $(CURDIR)/../..
DRIVERS 	  = $(NO-OS)/drivers
INCLUDE 	  = $(NO-OS)/include
SCRIPTS_DIR	  = $(NO-OS)/tools/scripts
PROJECTS_DIR 	  = $(NO-OS)/projects
SDK_WORKSPACE	  = $(PROJECT)/build
BUILD_DIR	  = $(PROJECT)/build
OBJECTS_DIR	  = $(BUILD_DIR)/obj
SCRIPTS_PATH 	  = $(SCRIPTS_DIR)/platform/$(PLATFORM)
LIBRARY_DIR 	  = $(BUILD_DIR)/bsp/$(PROCESSOR_CORE)/lib
PLATFORM_DRIVERS  = $(DRIVERS)/platform/LOCAL_PLATFORM
PROJECT 	  = $(PROJECTS_DIR)/$(TARGET)

# uncomment this line to include a custom config
#include $(SCRIPTS_DIR)/config.mk
include $(PROJECTS_DIR)/$(TARGET)/src.mk

# Default verbosity level is 0
VERBOSE ?=0
ifeq ($(VERBOSE),1)
MUTE =
NULL = 
else
MUTE = @
NULL = >/dev/null
endif

EXEC = release
TIMESTAMP = $(shell date +"%T")
define print
	@printf "\\e[32m[$(TIMESTAMP)]\\e[39m $(1)"
endef
define print_err
	@printf "\\e[32m[$(TIMESTAMP)]\\e[31m $(1)"
endef
CC ?= gcc

# Xilinx platform handling
ifeq (xilinx,$(strip $(PLATFORM)))

CFLAGS = -Wall 								\
	 -Wmissing-field-initializers					\
	 -Wclobbered 							\
	 -Wempty-body 							\
	 -Wignored-qualifiers 						\
	 -Wmissing-parameter-type					\
	 -Wold-style-declaration					\
	 -Woverride-init 						\
	 -Wsign-compare							\
	 -Wtype-limits							\
	 -Wuninitialized						\
	 -Wunused-but-set-parameter					\
	 -Wno-unused-parameter						\
	 -fdata-sections						\
	 -ffunction-sections 						\
	 -O2 								\
	 -g3								\
	 -MMD 								\
	 -MP								\
	 -lm						
	#-Werror

LDFLAGS = -Wl,-T							\
	  -Wl,$(BUILD_DIR)/app/src/lscript.ld -L $(LIBRARY_DIR)

# Zynq
ifeq (ps7_cortexa9_0,$(strip $(PROCESSOR_CORE)))

CC := arm-none-eabi-gcc

CFLAGS += -mcpu=cortex-a9 						\
	  -mfpu=vfpv3 							\
	  -mfloat-abi=hard

LDFLAGS += -specs=$(BUILD_DIR)/app/src/Xilinx.spec 			\
	   -mfpu=vfpv3							\
 	   -mfloat-abi=hard 						\
	   -mcpu=cortex-a9						\
	   -Wl,-build-id=none
endif
# Zynqmp 
ifeq (psu_cortexa53_0,$(strip $(PROCESSOR_CORE)))

CC := aarch64-none-elf-gcc

endif
# Microblaze
ifeq (sys_mb,$(strip $(PROCESSOR_CORE)))

CC := microblaze-xilinx-elf-gcc

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
LIBS =	-Wl,--start-group,-lxil,-lgcc,-lc,--end-group

endif

# Add the common include paths
INC_PATHS = $(foreach x, $(INCS), $(addprefix -I ,$(dir $(x))))
LIB_PATHS = $(foreach x, $(LIBRARY_DIR), $(addprefix -L ,$(x)))
EXTRA_INCS =

OBJS = $(SRCS:.c=.o)

# Compile all the source files
%.o : %.c
	$(call print, [CC] $(notdir $<) \n)
	$(MUTE)$(CC) $(CFLAGS) $(INC_PATHS) $(EXTRA_INCS)		\
	-c $(addprefix $(BUILD_DIR)/app/src/,$(notdir $<))		\
	-o $(addprefix $(OBJECTS_DIR)/,$(notdir $@))

# Link the resulted object files
$(EXEC): check-srcs $(subst LOCAL_PLATFORM,xilinx,$(OBJS))		\
		$(BUILD_DIR)/app/src/lscript.ld
	$(call print, [LD] $(shell ls $(OBJECTS_DIR)) \n)
	$(MUTE)$(CC) $(LDFLAGS) $(LIB_PATHS) 				\
	$(OBJECTS_DIR)/*.o						\
	$(LIBS) -o $(BUILD_DIR)/$(EXEC).elf

# Copy the source files from the no-Os repo to the local project
define copy_srcs
	$(MUTE)cp -r $(subst LOCAL_PLATFORM,$(1),$(SRCS))		\
		$(BUILD_DIR)/app/src/
	$(MUTE)cp -r $(subst LOCAL_PLATFORM,$(1),$(INCS))		\
		$(BUILD_DIR)/app/src/
endef

.DEFAULT_GOAL := all
all: 	xilinx

compile: $(EXEC)

create-obj-dir:
	$(MUTE)mkdir -p $(OBJECTS_DIR)
	
create-build-dir:
	$(MUTE)mkdir -p $(BUILD_DIR)

copy-srcs:
	$(call copy_srcs,$(PLATFORM))

check-srcs:
ifeq (xilinx,$(strip $(PLATFORM)))
# Replace the target sources (the default ones are the repository files) with
# the local project sources
	$(eval SRCS :=$(shell find $(BUILD_DIR)/app/src/ -name '*.c'))
	$(eval EXTRA_INCS :=-I $(BUILD_DIR)/bsp/$(PROCESSOR_CORE)/include)
	$(eval INC_PATHS :=-I $(BUILD_DIR)/app/src)
endif

clean:

	$(call print,Cleaning build workspace \n)
	$(MUTE)rm -rf temp
	$(MUTE)rm -rf $(BUILD_DIR)
	@$(MAKE) -s xil-pre-cleanup

eval-hardware:
# If the hardware file is not specified, start searching for one
# Check for .hdf files inside the project directory		
ifndef $(HARDWARE)
	$(eval HARDWARE = $(shell					\
	if [ -z $(HARDWARE) ]; then					\
		echo $(shell find $(PROJECT) -name "*.hdf") ;	\
	fi;))
	$(eval HARDWARE = $(shell					\
	if [ -z $(HARDWARE) ]; then					\
		echo $(shell find $(PROJECT) -name "*.sopcinfo") ;	\
	else								\
		echo $(shell find $(PROJECT) -name "*.hdf") ;	\
	fi;))
endif
# Assign the platform based on the hardware file extension
	$(call print,Evaluating hardware : $(notdir $(HARDWARE))\n)
	$(eval PLATFORM:= $(shell \
	if [ "$(findstring hdf,$(notdir $(HARDWARE)))" = "hdf" ]; then	\
		echo xilinx;						\
	elif [ "$(findstring sopcinfo,$(notdir $(HARDWARE)))" = "sopcinfo" ]; then\
		echo altera;						\
	else								\
		echo none;						\
	fi;))
	@ if [ "$(PLATFORM)" = "none" ]; then				\
		echo \\e[32m[$(TIMESTAMP)]\\e[31m Platform not found\\e[39m;\
		exit 1;							\
	fi;
	$(call print,Found platform : \e[33m$(PLATFORM)\e[39m\n)

pre-build:
	@$(MAKE) -s xil-pre-cleanup
	@$(MAKE) -s create-build-dir
	@$(MAKE) -s create-obj-dir

prepare-project:
	@ if [ "$(PLATFORM)" = "xilinx" ];then				\
		$(MAKE) -s xil-prepare-project	;			\
		$(MAKE) -s copy-srcs ;					\
	fi;
	@$(MAKE) -s compile

xilinx:	xil-pre-cleanup eval-hardware pre-build xil-prepare-project
	$(call copy_srcs,xilinx)
	@$(MAKE) -s compile
	$(MUTE)rm -rf $(BUILD_DIR)/SDK.log
	$(call print,Done \n)

xil-extract-hdf-info:
	$(MUTE)mkdir -p temp
	$(MUTE)cp $(HARDWARE) temp
	$(MUTE)rm -rf arch.txt
# Extract the processor name from the hdf file
	$(MUTE)xsct $(SCRIPTS_PATH)/read_hdf.tcl $(NULL)		\
		temp/$(notdir $(HARDWARE))

xil-update-hdf-info:
	$(eval PROCESSOR_CORE:= $(shell cat arch.txt))
	$(MUTE)rm -rf arch.txt

xil-pre-cleanup:
	$(MUTE)rm -rf \.Xil
	$(MUTE)rm -rf \.metadata

xil-prepare-project: xil-extract-hdf-info xil-update-hdf-info
	$(call print,Building hardware specification and bsp \n)
# Create the local sdk project
	$(MUTE)xsdk -batch -source $(SCRIPTS_PATH)/create_project.tcl	\
	$(SDK_WORKSPACE) $(HARDWARE) $(PROCESSOR_CORE) $(NULL)
	$(MUTE)mv $(PROJECT)/temp/system_top.bit $(BUILD_DIR)
	$(MUTE)rm -rf temp
	# Update in the linker script the heap size for microlbaze from 0x800 to 
	# 0x100000 
	@ if [ "$(PROCESSOR_CORE)" = "sys_mb" ]; then			\
		sed -i "s/_HEAP_SIZE : 0x800/_HEAP_SIZE : 0x100000/g"	\
		$(BUILD_DIR)/app/src/lscript.ld;			\
	fi;

re: clean all
