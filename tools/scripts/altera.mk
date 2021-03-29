#------------------------------------------------------------------------------
#                           ENVIRONMENT VARIABLES                              
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#                         The adjust-path macro
#
# If Make is launched from Windows through
# Windows Subsystem for Linux (WSL).  The adjust-path macro converts absolute windows 
# paths into unix style paths (Example: c:/dir -> /c/dir). 
# The adjust_path_mixed function converts WSL path to Windows path.
# This will ensure paths are readable by GNU Make.
#------------------------------------------------------------------------------

ifeq ($(OS), Windows_NT)
	ifneq ($(strip $(USE_LEGAY)),'y')
		WSL = wsl
		adjust_path = $(shell wsl wslpath $1)
		adjust_path_mixed = $(if $(call eq,$(shell echo $1 | wsl head -c 5),/mnt/),$(shell echo $1 | wsl sed 's/\/mnt\///g;s/\//:\//1'),$1)
	else
		adjust_path = $1
		adjust_path_mixed = $1
	endif
else # !Windows_NT
	adjust_path = $1
	adjust_path_mixed = $1
endif

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1

OBJECTS_DIR	= $(BUILD_DIR)/obj
TEMP_DIR	= $(BUILD_DIR)/tmp
BINARY		= $(BUILD_DIR)/$(PROJECT_NAME).elf

# Define the platform compiler switch
CFLAGS += -D ALTERA_PLATFORM

LSCRIPT = $(BUILD_DIR)/bsp/linker.x

LIB_PATHS += -L$(BUILD_DIR)/bsp

CC = nios2-elf-gcc 

LD = nios2-elf-g++

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

CFLAGS += -I"$(BUILD_DIR)/bsp/HAL/inc/sys"			\
		-I"$(BUILD_DIR)/bsp/drivers/inc"		\
		-I"$(BUILD_DIR)/bsp/HAL/inc"			\
		-I"$(BUILD_DIR)/bsp"	

#------------------------------------------------------------------------------
#                             Goals                         
#------------------------------------------------------------------------------


PHONY += altera_run
altera_run: all
	$(WSL) nios2-configure-sof *.sof
	$(WSL) nios2-download -r -g $(BINARY)
	nios2-terminal

$(PROJECT_TARGET):
	$(WSL) nios2-bsp hal "$(BUILD_DIR)/bsp" $(HARDWARE) --cpu-name sys_cpu
	$(WSL) $(MAKE) CFLAGS= -C $(call adjust_path, $(BUILD_DIR)/bsp)
	$(call set_one_time_rule,$@)

post_build:
	$(WSL) nios2-elf-insert $(BINARY) $(STAMP)
	-$(call copy_fun,sw.map,$(TEMP_DIR))
	-$(call remove_fun, sw.map)