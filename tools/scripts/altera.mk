#------------------------------------------------------------------------------
#                           ENVIRONMENT VARIABLES                              
#------------------------------------------------------------------------------

adjust_path = $1
adjust_path_mixed = $1

PROJECT_BUILD = $(BUILD_DIR)/app

OBJECTS_DIR	= $(BUILD_DIR)/obj
TEMP_DIR	= $(BUILD_DIR)/tmp
BINARY		= $(BUILD_DIR)/$(PROJECT_NAME).elf

# Define the platform compiler switch
CFLAGS += -D ALTERA_PLATFORM

LSCRIPT = $(BUILD_DIR)/bsp/linker.x

LIB_PATHS += -L$(BUILD_DIR)/bsp

CC = nios2-elf-gcc 
SIZE = nios2-elf-size

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
$(PLATFORM)_run: all
	$(WSL) nios2-configure-sof *.sof
	$(WSL) nios2-download -r -g $(BINARY)
	nios2-terminal

$(PLATFORM)_project:
	$(WSL) nios2-bsp hal "$(BUILD_DIR)/bsp" $(HARDWARE) --cpu-name sys_cpu
	$(WSL) $(MAKE) CFLAGS= -C $(call adjust_path, $(BUILD_DIR)/bsp)

$(PLATFORM)_post_build:
	$(WSL) nios2-elf-insert $(BINARY) $(STAMP)
	-$(call copy_file,sw.map,$(TEMP_DIR))
	$(call remove_file, sw.map)

$(PLATFORM)_reset:
