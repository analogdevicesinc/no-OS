#------------------------------------------------------------------------------
#                             EXPORTED VARIABLES                               
#------------------------------------------------------------------------------
# Exported variables used by subshells
export HARDWARE
export PLATFORM
export ARCH
export CFLAGS
export LDFLAGS
export CC
export LD

#------------------------------------------------------------------------------
#                           ENVIRONMENT VARIABLES                              
#------------------------------------------------------------------------------

NO-OS 		  ?= $(CURDIR)/../..
#Needed by iio library
#Should be exported after attribution. Otherwise, is set as null and the
#the attribution will not work
export NO-OS

DRIVERS 	  ?= $(NO-OS)/drivers
INCLUDE 	  ?= $(NO-OS)/include
LIBRARIES	  ?= $(NO-OS)/libraries
SCRIPTS_DIR	  ?= $(NO-OS)/tools/scripts
PROJECTS_DIR 	  ?= $(NO-OS)/projects
SDK_WORKSPACE	  ?= $(PROJECT)/build
BUILD_DIR	  ?= $(PROJECT)/build
OBJECTS_DIR	  ?= $(BUILD_DIR)/obj
TEMP_DIR	  ?= $(BUILD_DIR)/tmp
SCRIPTS_PATH 	  ?= $(SCRIPTS_DIR)/platform/$(PLATFORM)
PLATFORM_DRIVERS  ?= $(DRIVERS)/platform/LOCAL_PLATFORM
PROJECT 	  ?= $(PROJECTS_DIR)/$(TARGET)

#------------------------------------------------------------------------------
#                             MAKEFILE INCLUDES                                
#------------------------------------------------------------------------------

ifndef SRCS
# Uncomment this line to include a custom config
#include $(SCRIPTS_DIR)/config.mk
include $(PROJECTS_DIR)/$(TARGET)/src.mk
endif

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
#                                ECHO HELPER                                   
#------------------------------------------------------------------------------
EXEC = release
TIMESTAMP = $(shell date +"%T")
define print
	printf "\\e[32m[$(TIMESTAMP)]\\e[39m $(1)"
endef
define print_err
	printf "\\e[32m[$(TIMESTAMP)]\\e[31m $(1)"
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
CFLAGS += -Wall								\
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

LIB_TINYIIOD_PATH = ""
LIB_TINYIIOD = ""
TINYIIOD_STD_TYPES = ""
ifeq (y,$(strip $(TINYIIOD)))
CFLAGS += -D IIO_SUPPORT
LIB_TINYIIOD_PATH = $(LIBRARIES)/libtinyiiod/build
LIB_TINYIIOD = tinyiiod
TINYIIOD_STD_TYPES = _USE_STD_INT_TYPES
CFLAGS += -D $(TINYIIOD_STD_TYPES)
endif

ifeq (y,$(strip $(MBEDTLS)))
#Specify configuration file to build mbedtls
CFLAGS += -I $(NO-OS)/network/transport -D MBEDTLS_CONFIG_FILE='"noos_mbedtls_config.h"'
#Add mbedtls include directory to inlcude path
INCLUDE += $(NO-OS)/libraries/mbedtls/include
endif

#------------------------------------------------------------------------------
#                            COMMON LINKER FLAGS                               
#------------------------------------------------------------------------------
LDFLAGS = -T $(LSCRIPT)							\
	  $(LIBS)

#------------------------------------------------------------------------------
#                             PLATFORM HANDLING                                
#------------------------------------------------------------------------------

########|----------------------------------------------------------------------
########|                           XILINX                                     
########|----------------------------------------------------------------------
ifeq (xilinx,$(strip $(PLATFORM)))

# Define the platform compiler switch
CFLAGS += -DXILINX_PLATFORM						\
	 -fdata-sections						\
	 -ffunction-sections 						\
	 -O2								\
	 -g3
# Xilinx's generated linker script path
LSCRIPT := $(BUILD_DIR)/app/src/lscript.ld
# Xilinx's generate bsp library path
LIBS += -L $(BUILD_DIR)/bsp/$(ARCH)/lib
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

CC := microblaze-xilinx-elf-gcc

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
LIBS +=	-Wl,--start-group,-lxil,-lgcc,-lc,--end-group

else
ifeq (altera,$(strip $(PLATFORM)))

# Define the platform compiler switch
CFLAGS += -D ALTERA_PLATFORM

LSCRIPT := $(BUILD_DIR)/bsp/linker.x

LIBS += -L $(BUILD_DIR)/bsp

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

LDFLAGS += -msys-crt0='$(BUILD_DIR)/bsp//obj/HAL/src/crt0.o'		\
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
INC_PATHS +=-I $(BUILD_DIR)/app/src

OBJS = $(SRCS:.c=.o)

# Compile all the source files
%.o : %.c
	@$(call print,[CC] $(notdir $<) \n)
	$(CC) $(CFLAGS)	$(INC_PATHS) $(EXTRA_INCS)			\
	-c $(addprefix $(BUILD_DIR)/app/src/,$(notdir $<))		\
	-o $(addprefix $(OBJECTS_DIR)/,$(notdir $@))

# Link the resulted object files
$(EXEC): $(subst LOCAL_PLATFORM,xilinx,$(OBJS))				\
		$(LSCRIPT)
	$(call print,[LD] $(shell ls $(OBJECTS_DIR)) \n)
	$(MUTE)$(LD) $(LDFLAGS) $(LIB_PATHS) 				\
	$(OBJECTS_DIR)/*.o						\
	$(LIBS) -o $(BUILD_DIR)/$(EXEC).elf

.DEFAULT_GOAL := all

.SILENT:all
all: pre-build eval-hardware bsp 
	$(MAKE) -s copy-srcs
	$(MAKE) -s compile
	$(MAKE) -s post-build
	$(call print,Done \n)

.SILENT:bsp
bsp: 
	$(call print,Platform : \e[33m$(PLATFORM)\e[39m\n)
	@ if [ "$(PLATFORM)" = "xilinx" ];then				\
		$(eval export ARCH:= $(shell cat $(TEMP_DIR)/arch.txt))	\
		$(MAKE) -s xilinx-bsp;					\
	elif [ "$(PLATFORM)" = "altera" ];then				\
		$(MAKE) -s altera-bsp;					\
	else								\
		$(call print_err,Can't generate the bsp\n);		\
		exit 1;							\
	fi;

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
	$(MAKE) -s libs
	$(MAKE) -s $(EXEC)

.SILENT:create-build-dirs
create-build-dirs:
	mkdir -p $(BUILD_DIR)/app
	mkdir -p $(BUILD_DIR)/app/src
	mkdir -p $(OBJECTS_DIR)
	mkdir -p $(TEMP_DIR)
	

# Copy the source files from the no-Os repo to the local project
.SILENT:copy-srcs
copy-srcs:
	cp -r $(subst LOCAL_PLATFORM,$(PLATFORM),$(SRCS))	\
		$(BUILD_DIR)/app/src/
	cp -r $(subst LOCAL_PLATFORM,$(PLATFORM),$(INCS))	\
		$(BUILD_DIR)/app/src/

.SILENT:clean
clean:
	$(call print,Cleaning build workspace \n)
	rm -rf $(BUILD_DIR)
	# Clean mbedtls only if submodule exists
	( ! test -f $(LIBRARIES)/mbedtls/Makefile ) || \
	$(MAKE) -C $(LIBRARIES)/mbedtls clean

.SILENT:clean_libs
clean_libs:
	-$(CLEAN_IIO)

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
	$(call print,Evaluating hardware : $(notdir $(HARDWARE))\n)
	$(eval PLATFORM:= $(shell \
	if [ "$(findstring hdf,$(notdir $(HARDWARE)))" = "hdf" ]; then	\
		echo xilinx;						\
	elif [ "$(findstring sopcinfo,$(notdir $(HARDWARE)))" 		\
		= "sopcinfo" ]; then					\
		echo altera;						\
	else								\
		echo none;						\
	fi;))
	@ if [ "$(PLATFORM)" = "xilinx" ];then				\
		$(MAKE) -s xilinx-read-hdf;				\
	elif [ "$(PLATFORM)" = "altera" ];then				\
		echo nios2 > $(TEMP_DIR)/arch.txt;			\
	elif [ "$(PLATFORM)" = "none" ]; then				\
		$(call print_err,Platform not found\n)			\
		exit 1;							\
	fi;

.SILENT:altera-bsp
altera-bsp:
	nios2-bsp hal $(BUILD_DIR)/bsp $(HARDWARE) --cpu-name sys_cpu $(NULL)
	$(MAKE) -C $(BUILD_DIR)/bsp $(NULL)

.SILENT:altera-elf
altera-elf:
	nios2-elf-insert $(BUILD_DIR)/$(EXEC).elf $(STAMP)

.SILENT:libs
libs: $(LIB_TARGETS)
ifeq (y,$(strip $(TINYIIOD)))
	@$(MAKE) -C $(LIBRARIES)/libtinyiiod re
endif
ifeq (y,$(strip $(MBEDTLS)))
	@$(MAKE) -C $(LIBRARIES)/mbedtls lib
endif
ifeq (y,$(strip $(TINYIIOD)))
LIBS += -L $(LIB_TINYIIOD_PATH) -l$(LIB_TINYIIOD)
endif
ifeq (y,$(strip $(MBEDTLS)))
LIBS += -L $(LIBRARIES)/mbedtls/library -lmbedtls -lmbedx509 -lmbedcrypto
endif

.SILENT:pre-build
pre-build:
	@$(MAKE) -s create-build-dirs

.SILENT:post-build
post-build:
	@ if [ "$(PLATFORM)" = "xilinx" ];then				\
		rm -rf \.Xil;						\
		rm -rf \.metadata;					\
	else								\
		$(MAKE) -s altera-elf;					\
		mv -f sw.map $(TEMP_DIR);				\
	fi;

# Upload the bitstream and elf file to the board
.SILENT:run
run: eval-hardware
	@ if [ "$(PLATFORM)" = "xilinx" ];then				\
		xsdb $(SCRIPTS_PATH)/upload.tcl				\
			$(shell cat $(TEMP_DIR)/arch.txt) 		\
			$(BUILD_DIR)/hw/system_top.bit			\
			$(BUILD_DIR)/$(EXEC).elf			\
			$(BUILD_DIR)/hw	;				\
	elif [ "$(PLATFORM)" = "altera" ];then				\
		nios2-configure-sof *.sof;				\
		nios2-download -r -g $(BUILD_DIR)/$(EXEC).elf;		\
		nios2-terminal;						\
	elif [ "$(PLATFORM)" = "none" ]; then				\
		$(call print_err,Platform not found\n)			\
		exit 1;							\
	fi;



include_path_cmd=xsct -eval "setws $(SDK_WORKSPACE); sdk configapp -app app include-path $1";
compiler_define_cmd= xsct -eval "setws $(SDK_WORKSPACE); sdk configapp -app app define-compiler-symbols $1";
lib_serach_path_cmd= xsct -eval "setws $(SDK_WORKSPACE); sdk configapp -app app library-search-path $1";
add_library_cmd= xsct -eval "setws $(SDK_WORKSPACE); sdk configapp -app app libraries $1";

INC_PATHS_WITHOUT_I = $(subst -I,,$(INC_PATHS))
ADD_INCLUDE_PATHS=$(foreach dir, $(INC_PATHS_WITHOUT_I), $(call include_path_cmd,$(dir)))

FLAGS_WITHOUT_D = $(sort $(subst -D,,$(filter -D%, $(CFLAGS))))
ADD_COMPILER_DEFINES = $(foreach flag, $(FLAGS_WITHOUT_D), $(call compiler_define_cmd,$(flag)))

ADD_LIBRARIES = $(foreach lib, $(LIBRARIES), $(call add_library_cmd,$(lib)))

LIBRARIES_PATH_WITHOUT_L = $(subst -L,,$(LIB_PATHS))
ADD_LIBRARIES_PATH = $(foreach path, $(LIBRARIES_PATH_WITHOUT_L), $(call lib_serach_path_cmd,$(path)))

# Extract the architecture from the hdf file
.SILENT:xilinx-read-hdf
xilinx-read-hdf:
	cp $(HARDWARE) $(TEMP_DIR)
	xsct $(SCRIPTS_PATH)/read_hdf.tcl $(NULL)			\
		$(TEMP_DIR) $(TEMP_DIR)/$(notdir $(HARDWARE))

.SILENT:xilinx-bsp
xilinx-bsp:
	@ if [ ! -d "$(BUILD_DIR)/bsp" ];then				\
	$(call print,Building hardware specification and bsp \n);	\
	xsdk -batch -source $(SCRIPTS_PATH)/create_project.tcl		\
		$(SDK_WORKSPACE) $(HARDWARE) $(ARCH) 			\
		$(LIB_TINYIIOD_PATH) $(LIB_TINYIIOD)			\
		$(TINYIIOD_STD_TYPES) $(NULL);				\
	fi;
	$(ADD_INCLUDE_PATHS)
	$(ADD_COMPILER_DEFINES)
	$(ADD_LIBRARIES_PATH)
	$(ADD_LIBRARIES)

# Update the linker script the heap size for microlbaze from 0x800 to 
# 0x100000 
	@ if [ "$(ARCH)" = "sys_mb" ]; then				\
		sed -i "s/_HEAP_SIZE : 0x800/_HEAP_SIZE : 0x100000/g"	\
		$(BUILD_DIR)/app/src/lscript.ld;			\
	else								\
		sed -i "s/_HEAP_SIZE : 0x2000/_HEAP_SIZE : 0x100000/g"	\
		$(BUILD_DIR)/app/src/lscript.ld;			\
	fi;
	$(MUTE)rm -rf $(BUILD_DIR)/SDK.log

re: clean all
