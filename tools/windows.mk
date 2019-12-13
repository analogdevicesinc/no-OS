#------------------------------------------------------------------------------
#                             EXPORTED VARIABLES                               
#------------------------------------------------------------------------------
# Exported variables used by subshells
export HARDWARE
export PLATFORM
export ARCH
export CC

#------------------------------------------------------------------------------
#                           ENVIRONMENT VARIABLES                              
#------------------------------------------------------------------------------
NO-OS 		  = $(subst /c/,C:/,$(CURDIR)/../..)
DRIVERS 	  = $(NO-OS)/drivers
INCLUDE 	  = $(NO-OS)/include
SCRIPTS_DIR	  = $(NO-OS)/tools/scripts
PROJECTS_DIR 	  = $(NO-OS)/projects
SDK_WORKSPACE	  = $(PROJECT)/build
BUILD_DIR	  = $(PROJECT)/build
OBJECTS_DIR	  = $(BUILD_DIR)/obj
TEMP_DIR	  = $(BUILD_DIR)/tmp
SCRIPTS_PATH 	  = $(SCRIPTS_DIR)/platform/$(PLATFORM)
PLATFORM_DRIVERS  = $(DRIVERS)/platform/LOCAL_PLATFORM
PROJECT 	  = $(PROJECTS_DIR)/$(TARGET)

#------------------------------------------------------------------------------
#                             MAKEFILE INCLUDES                                
#------------------------------------------------------------------------------
# Uncomment this line to include a custom config
#include $(SCRIPTS_DIR)/config.mk
include $(PROJECTS_DIR)/$(TARGET)/src.mk

#------------------------------------------------------------------------------
#                               VERBOSE LEVEL                                  
#------------------------------------------------------------------------------
# Default verbosity level is 0
VERBOSE ?=0
ifeq ($(VERBOSE),1)
MUTE =
NULL = 
else
MUTE = @
NULL = >/dev/null
endif

#------------------------------------------------------------------------------
#                              COMMAND WRAPEPRS                                  
#------------------------------------------------------------------------------

MKDIR = @powershell New-Item -ItemType directory -ErrorAction Ignore -Path 
RM = powershell Remove-Item -Confirm:\$$false -Force -Recurse -ErrorAction Ignore -Path 
TIMESTAMP = Get-Date -Format "HH:mm.ss"

#------------------------------------------------------------------------------
#                                ECHO HELPER                                   
#------------------------------------------------------------------------------
EXEC = release
define copy
	powershell Copy-Item -Path $(1) -Destination $(2)
endef
define print
	@powershell Write-Host 
	@powershell Write-Host "--------" -f green
	@powershell $(TIMESTAMP)
	@powershell Write-Host "--------" -f green
	@powershell Write-Host 
	@powershell Write-Host $(1)
	@powershell Write-Host 
endef
define print_err
	@powershell Write-Host "--------" -f red
	@powershell $(TIMESTAMP)
	@powershell Write-Host "--------" -f red
	@powershell Write-Host $(1) -f red
	@powershell Write-Host 
endef

#------------------------------------------------------------------------------
#                              DEFAULT COMPILER                                
#------------------------------------------------------------------------------
CC ?= gcc

#------------------------------------------------------------------------------
#                              DEFAULT LINKER                                  
#------------------------------------------------------------------------------
LD ?= $(CC)

#------------------------------------------------------------------------------
#                            COMMON COMPILER FLAGS                             
#------------------------------------------------------------------------------
CFLAGS = -Wall 								\
	 -Wmissing-field-initializers					\
	 -Wclobbered 							\
	 -Wempty-body 							\
	 -Wignored-qualifiers 						\
	 -Wmissing-parameter-type					\
	 -Wno-format  							\
	 -Wold-style-declaration					\
	 -Woverride-init 						\
	 -Wsign-compare							\
	 -Wtype-limits							\
	 -Wuninitialized						\
	 -Wunused-but-set-parameter					\
	 -Wno-unused-parameter						\
	 -MMD 								\
	 -MP								\
	 -lm						
	#-Werror

#------------------------------------------------------------------------------
#                            COMMON LINKER FLAGS                               
#------------------------------------------------------------------------------
LDFLAGS = -T $(LSCRIPT)							\
	  -L $(LIBRARY_DIRS)

#------------------------------------------------------------------------------
#                             PLATFORM HANDLING                                
#------------------------------------------------------------------------------

########|----------------------------------------------------------------------
########|                           XILINX                                     
########|----------------------------------------------------------------------
ifeq (xilinx,$(strip $(PLATFORM)))

# Define the platform compiler switch
CFLAGS += -D XILINX_PLATFORM						\
	 -fdata-sections						\
	 -ffunction-sections 						\
	 -O2								\
	 -g3
# Xilinx's generated linker script path
LSCRIPT := $(BUILD_DIR)/app/src/lscript.ld
# Xilinx's generate bsp library path
LIBRARY_DIRS := $(BUILD_DIR)/bsp/$(ARCH)/lib
# Xilinx's bsp include path
EXTRA_INCS := -I $(BUILD_DIR)/bsp/$(ARCH)/include
################|--------------------------------------------------------------
################|                   Zynq                                       
################|--------------------------------------------------------------
ifeq (ps7_cortexa9_0,$(strip $(ARCH)))

CC := arm-none-eabi-gcc

LD := $(CC)

CFLAGS += -mcpu=cortex-a9 						\
	  -mfpu=vfpv3 							\
	  -mfloat-abi=hard

LDFLAGS += -specs=$(BUILD_DIR)/app/src/Xilinx.spec 			\
	   -mfpu=vfpv3							\
 	   -mfloat-abi=hard 						\
	   -mcpu=cortex-a9						\
	   -Wl,-build-id=none
endif
################|--------------------------------------------------------------
################|                   ZynqMP                                     
################|--------------------------------------------------------------
ifeq (psu_cortexa53_0,$(strip $(ARCH)))

CC := aarch64-none-elf-gcc

LD := $(CC)

endif
################|--------------------------------------------------------------
################|                  Microblaze                                  
################|--------------------------------------------------------------
ifeq (sys_mb,$(strip $(ARCH)))

CC := mb-gcc

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
LIBS =	-Wl,--start-group,-lxil,-lgcc,-lc,--end-group

else
ifeq (altera,$(strip $(PLATFORM)))

# Define the platform compiler switch
CFLAGS += -D ALTERA_PLATFORM

LSCRIPT := $(BUILD_DIR)/bsp/linker.x

LIBRARY_DIRS := $(BUILD_DIR)/bsp

CC := nios2-elf-gcc 

LD := nios2-elf-g++

CFLAGS += -xc								\
	  -pipe								\
	  -O3								\
	  -g								\
	  -mno-hw-div							\
	  -mhw-mul							\
	  -mno-hw-mulx							\
	  -mgpopt=global						\
	  -D__hal__							\
	  -DALT_NO_INSTRUCTION_EMULATION 				\
	  -DALTERA_TRIPLE_SPEED_MAC 					\
	  -DALT_SINGLE_THREADED 					\
	  -DALTERA_AUTONEG_TIMEOUT_THRESHOLD=2500 			\
	  -DALTERA_CHECKLINK_TIMEOUT_THRESHOLD=10000 			\
	  -DALTERA_NOMDIO_TIMEOUT_THRESHOLD=1000000  			\
	  -DALTERA							\
	  -DNIOS_II

LDFLAGS += -msys-crt0='$(BUILD_DIR)/bsp/HAL/src/crt0.o'		\
	   -msys-lib=hal_bsp 						\
	   -DALTERA							\
	   -DNIOS_II							\
	   -mno-hw-div							\
	   -mhw-mul							\
	   -mno-hw-mulx							\
	   -mgpopt=global						\
	   -lm								\
	   -msys-lib=m							\
	   -Wl,-Map=sw.map

STAMP +=   --thread_model hal						\
	   --cpu_name sys_cpu						\
	   --qsys true							\
	   --simulation_enabled false					\
	   --id 182193580						\
	   --sidp 0x101814e8						\
	   --timestamp 1520874786					\
	   --stderr_dev sys_uart					\
	   --stdin_dev sys_uart						\
	   --stdout_dev sys_uart					\
	   --sopc_system_name system_bd					\
	   --sopcinfo $(HARDWARE)
endif
endif

# Add the common include paths
INC_PATHS :=-I $(BUILD_DIR)/app/src
LIB_PATHS = $(foreach x, $(LIBRARY_DIRS), $(addprefix -L ,$(x)))

OBJS = $(SRCS:.c=.o)

# Compile all the source files
%.o : %.c
	@$(call print,[CC] $(notdir $<))
	$(CC) $(CFLAGS)	$(INC_PATHS) $(EXTRA_INCS)			\
	-c $(addprefix $(BUILD_DIR)/app/src/,$(notdir $<))		\
	-o $(addprefix $(OBJECTS_DIR)/,$(notdir $@))

# Link the resulted object files
$(EXEC): 
	make $(subst LOCAL_PLATFORM,xilinx,$(OBJS)) $(LSCRIPT)
	$(call print,[LD] )
	@powershell Get-ChildItem -Path $(OBJECTS_DIR) -Name
	$(MUTE)$(LD) $(LDFLAGS) $(LIB_PATHS) 				\
	$(OBJECTS_DIR)/*.o						\
	$(LIBS) -o $(BUILD_DIR)/$(EXEC).elf

.DEFAULT_GOAL := all

.SILENT:all
all: pre-build eval-hardware bsp 
	$(MAKE) -s copy-srcs
	$(MAKE) -s compile
	$(MAKE) -s post-build
	$(call print,Done )

.SILENT:get-arch
get-arch:
	@powershell Get-Content $(TEMP_DIR)/arch.txt
	@powershell Remove-Item $(TEMP_DIR)/arch.txt

.SILENT:bsp
bsp:
	$(call print,Platform : $(PLATFORM))
	$(eval export ARCH:= $(shell make -s get-arch))
	$(call print,Building hardware specification and bsp );
	$(shell \
	if [ "$(PLATFORM)" = "xilinx" ];then				\
		echo make -s xilinx-bsp;					\
	elif [ "$(PLATFORM)" = "altera" ];then				\
		echo make -s altera-bsp;					\
	else								\
		$(call print_err,Unable to generate the bsp);		\
		exit 1;							\
	fi;)

.SILENT:compile	
compile: 
	$(eval export EXTRA_INCS:= $(shell \
	if [ "$(PLATFORM)" = "xilinx" ]; then	\
		echo "-I $(BUILD_DIR)/bsp/$(ARCH)/include";\
	elif [ "$(PLATFORM)" = "altera" ];then				\
		echo "-I $(BUILD_DIR)/bsp/HAL/inc/sys			\
		      -I $(BUILD_DIR)/bsp/drivers/inc			\
		      -I $(BUILD_DIR)/bsp/HAL/inc			\
		      -I $(BUILD_DIR)/bsp";				\
	fi;))
	$(MAKE) -s $(EXEC)

SILENT:create-build-dirs
create-build-dirs:
	$(MKDIR) $(BUILD_DIR) $(NULL)
	$(MKDIR) $(BUILD_DIR)/app $(NULL)
	$(MKDIR) $(BUILD_DIR)/app/src $(NULL)
	$(MKDIR) $(BUILD_DIR)/bsp $(NULL)
	$(MKDIR) $(OBJECTS_DIR) $(NULL)
	$(MKDIR) $(TEMP_DIR) $(NULL)

# Copy the source files from the no-Os repo to the local project
.SILENT:copy-srcs
copy-srcs:
	$(call print,Copying sources)
	for src in $(subst LOCAL_PLATFORM,$(PLATFORM),$(SRCS));\
	do\
		powershell Copy-Item -Path $$src -Destination $(BUILD_DIR)/app/src;\
	done
	for inc in $(subst LOCAL_PLATFORM,$(PLATFORM),$(INCS));\
	do\
		powershell Copy-Item -Path $$inc -Destination $(BUILD_DIR)/app/src;\
	done

.SILENT:clean
clean:
	$(call print,Cleaning build workspace )
	$(RM) $(BUILD_DIR)

# If the hardware file is not specified, start searching for one
# Check for .hdf files inside the project directory
.SILENT:eval-hardware
eval-hardware:	
ifndef HARDWARE
	$(eval HARDWARE = $(shell					\
	if [ -z $(HARDWARE) ]; then					\
		echo $(shell find $(PROJECT) -maxdepth 1		\
			-name "*.hdf") ;				\
	fi;))
	$(eval HARDWARE = $(shell					\
	if [ -z $(HARDWARE) ]; then					\
		echo $(shell find $(PROJECT) -maxdepth 1		\
			-name "*.sopcinfo") ;				\
	else								\
		echo $(shell find $(PROJECT) -maxdepth 1		\
			-name "*.hdf") ;				\
	fi;))
endif
# Assign the platform based on the hardware file extension
	$(call print,Evaluating hardware : $(notdir $(HARDWARE)))
	$(eval PLATFORM:= $(shell \
	if [ "$(findstring hdf,$(notdir $(HARDWARE)))" = "hdf" ]; then	\
		echo xilinx;						\
	elif [ "$(findstring sopcinfo,$(notdir $(HARDWARE)))" 		\
		= "sopcinfo" ]; then					\
		echo altera;						\
	else								\
		echo none;						\
	fi;))
	$(shell \
	if [ "$(PLATFORM)" = "xilinx" ];then				\
		echo make -s xilinx-read-hdf;				\
	elif [ "$(PLATFORM)" = "altera" ];then				\
		echo make -s altera-read-sopcinfo;		\
	elif [ "$(PLATFORM)" = "none" ]; then				\
		$(call print_err,Platform not found)			\
		exit 1;							\
	fi;)

.SILENT:altera-bsp
altera-bsp:
	nios2-bsp-create-settings			\
	--type hal					\
	--bsp-dir $(BUILD_DIR)/bsp			\
	--cpu-name sys_cpu			\
	--settings settings			\
	--script $(ALTERA_BIN)/bsp-set-defaults.tcl	 \
	--sopc $(HARDWARE)			\
	$(NULL)
	nios2-bsp-generate-files		\
	--settings settings			\
	--bsp-dir $(BUILD_DIR)/bsp			\
	--silent\
	$(NULL)	
	powershell Copy-Item -Path $(SCRIPTS_PATH)/bsp.mk $(BUILD_DIR)/bsp/Makefile
	$(MAKE) -C $(BUILD_DIR)/bsp

.SILENT:altera-elf
altera-elf:
	nios2-elf-insert $(BUILD_DIR)/$(EXEC).elf $(STAMP)

.SILENT:altera-read-sopcinfo
altera-read-sopcinfo:
# Right now only nios2 is supported
	@powershell New-Item -Path $(TEMP_DIR)/arch.txt $(NULL)
	@powershell Set-Content -Path $(TEMP_DIR)/arch.txt 'nios2'

altera-run:
	$(call print,Uploading... This may take a while)
	@$(NIOS_SHELL)/Nios\ II\ Command\ Shell\.bat nios2-configure-sof *.sof > /dev/null
	@$(NIOS_SHELL)/Nios\ II\ Command\ Shell\.bat nios2-download -r -g $(BUILD_DIR)/$(EXEC).elf > /dev/null
	@$(NIOS_SHELL)/Nios\ II\ Command\ Shell\.bat nios2-terminal

.SILENT:pre-build
pre-build:
	@$(MAKE) -s create-build-dirs

.SILENT:post-build
post-build:
	$(shell \
		if [ "$(PLATFORM)" = "altera" ];then						\
			powershell Move-Item -Path sw.map -Destination $(TEMP_DIR);	\
			echo make -s altera-elf;					\
		fi;)

# Upload the bitstream and elf file to the board
.SILENT:run
run: eval-hardware
	$(eval export ARCH:= $(shell make -s get-arch))
	$(shell \
	if [ "$(PLATFORM)" = "xilinx" ];then				\
			echo make -s xilinx-run;	\
	elif [ "$(PLATFORM)" = "altera" ];then				\
			echo make -s altera-run;			\
	elif [ "$(PLATFORM)" = "none" ]; then				\
		$(call print_err,Platform not found)			\
		exit 1;							\
	fi;)

# Extract the architecture from the hdf file
.SILENT:xilinx-read-hdf
xilinx-read-hdf:
	$(call copy,$(HARDWARE),$(TEMP_DIR))
	xsct.bat $(SCRIPTS_PATH)/read_hdf.tcl			\
		$(TEMP_DIR) $(TEMP_DIR)/$(notdir $(HARDWARE)) \
		$(NULL)

.SILENT:xilinx-replace-heap
xilinx-replace-heap:
	powershell \( \( Get-Content 						\
		-Path $(BUILD_DIR)/app/src/lscript.ld -Raw \) 	\
		-Replace \'_HEAP_SIZE : 0x800\',				\
		\'_HEAP_SIZE : 0x100000\' \) \| 				\
		Set-Content -Path $(BUILD_DIR)/app/src/lscript.ld

.SILENT:xilinx-bsp
xilinx-bsp:
	xsdk -batch -source $(SCRIPTS_PATH)/create_project.tcl \
		$(SDK_WORKSPACE) $(HARDWARE) $(ARCH) $(NULL);
# Update the linker script the heap size for microlbaze from 0x800 to 
# 0x100000 
	$(shell \
		if [ "$(ARCH)" = "sys_mb" ]; then				\
			echo make -s xilinx-replace-heap;					\
		fi;\
	)
	$(RM) .Xil;	

xilinx-run:
	$(call print,Uploading... This may take a while)
	xsdb.bat $(SCRIPTS_PATH)/upload.tcl				\
		$(ARCH) 							\
		$(BUILD_DIR)/hw/system_top.bit			\
		$(BUILD_DIR)/$(EXEC).elf			\
		$(BUILD_DIR)/hw	$(NULL);				\

re: clean all