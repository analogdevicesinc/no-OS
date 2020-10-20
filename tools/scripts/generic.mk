#------------------------------------------------------------------------------
#                             EXPORTED VARIABLES                               
#------------------------------------------------------------------------------
# Used by nested Makefils (mbedtls, fatfs, iio)
export CFLAGS
export CC
export AR

#------------------------------------------------------------------------------
#                              UTIL FUNCTIONS
#------------------------------------------------------------------------------

#	WINDOWS
ifeq ($(OS), Windows_NT)
SHELL=cmd
copy_fun = copy /Y /B "$(subst /,\,$1)" "$(subst /,\,$2)"
copy_folder = xcopy /S /Y /C /I "$(subst /,\,$1)" "$(subst /,\,$2)"
remove_fun = del /S /Q $(subst /,\,$1)
remove_dir = rd /S /Q "$(subst /,\,$1)"
mk_dir = md $(subst /,\,$1)
read_file = type $(subst /,\,$1)
#	LINUX
else
copy_fun = cp $1 $2
copy_folder = cp -r $1 $2
remove_fun = rm -rf $1
remove_dir = rm -rf $1
mk_dir = mkdir -p $1
read_file = cat $1
endif

# recursive wildcard
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#Creates file with the specified name
set_one_time_rule = echo Target file. Do not delete > $1

#------------------------------------------------------------------------------
#                           ENVIRONMENT VARIABLES                              
#------------------------------------------------------------------------------

NO-OS			?= $(realpath ../..)
#Need in libraries
export NO-OS
PROJECT			?= $(realpath .)
DRIVERS			?= $(NO-OS)/drivers
INCLUDE			?= $(NO-OS)/include
PLATFORM_DRIVERS	?= $(NO-OS)/drivers/platform/$(PLATFORM)

#USED IN MAKEFILE
PROJECT_NAME		= $(notdir $(PROJECT))
BUILD_DIR		= $(PROJECT)/build
OBJECTS_DIR		= $(BUILD_DIR)/objs
WORKSPACE		?= $(BUILD_DIR)
PLATFORM_TOOLS		= $(NO-OS)/tools/scripts/platform/$(PLATFORM)
BINARY			= $(BUILD_DIR)/$(PROJECT_NAME)

ifneq ($(words $(NO-OS)), 1)
$(error $(ENDL)ERROR:$(ENDL)\
Plese clone no-os in a path without spaces,$(ENDL)\
otherwise the makefile will not work well.$(ENDL)\
Current path is: $(NO-OS)$(ENDL))
endif

#------------------------------------------------------------------------------
#                          INCLUDE SPECIFIC MAKEFILES
#------------------------------------------------------------------------------

HARDWARE ?= $(wildcard *.hdf) $(wildcard *.sopcinfo) $(wildcard pinmux_config.c)

#If platform not set get it from HARDWARE file
ifeq '' '$(PLATFORM)'
ifneq '' '$(findstring hdf,$(HARDWARE))'
PLATFORM = xilinx
else
ifneq '' '$(findstring sopcinfo,$(HARDWARE))'
PLATFORM = altera
else
ifneq '' '$(findstring pinmux_config.c,$(HARDWARE))'
PLATFORM = aducm3029
else
$(error No HARDWARE found)
endif
endif
endif
endif

ifeq 'xilinx' '$(PLATFORM)'
HARDWARE := $(filter %.hdf, $(HARDWARE))
ifeq '' '$(HARDWARE)'
$(error 'No HARDWARE for xilinx found. Add .hdf file')
endif
include $(NO-OS)/tools/scripts/xilinx.mk
endif

ifeq 'altera' '$(PLATFORM)'
HARDWARE := $(filter %.sopcinfo, $(HARDWARE))
ifeq '' '$(HARDWARE)'
$(error 'No HARDWARE for altera found. Add .sopcinfo file')
endif
include $(NO-OS)/tools/scripts/altera.mk
endif

ifeq 'aducm3029' '$(PLATFORM)'
HARDWARE := $(filter pinmux_config.c, $(HARDWARE))
ifeq '' '$(HARDWARE)'
$(error 'No HARDWARE for aducm3029 found. Add pinmux_config.c file')
endif
include $(NO-OS)/tools/scripts/aducm.mk
endif

$(info $(HARDWARE) found, building for $(PLATFORM))

#------------------------------------------------------------------------------
#                          COMMON INITIALIZATION
#------------------------------------------------------------------------------

# Transforme full path to relative path to be used in build
# $(PROJECT)/something -> srcs/something
# $(NO-OS)/something -> noos/something
# $(PLATFORM_TOOLS)/something -> aducm3029/something TODO test without these
get_relative_path = $(patsubst $(NO-OS)%,noos%,$1)

# Transforme relative path to full path in order to find the needed .c files
# Reverse of get_relative_path
get_full_path = $(patsubst noos%,$(NO-OS)%,$1)

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
CFLAGS += -DENABLE_IIO_NETWORK
endif

include $(NO-OS)/tools/scripts/libraries.mk

# Get all .c and .h files from SRC_DIRS
SRCS     += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.c))
INCS     += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.h))

# Recursive ignored files. If a directory is in the variable IGNORED_FILES,
# all files from inside the directory will be ignored
ALL_IGNORED_FILES = $(foreach dir, $(IGNORED_FILES), $(call rwildcard, $(dir),*))

# Remove ignored files
SRCS     := $(filter-out $(ALL_IGNORED_FILES),$(SRCS))
INCS     := $(filter-out $(ALL_IGNORED_FILES),$(INCS))

REL_SRCS = $(foreach file, $(SRCS),  $(OBJECTS_DIR)/$(call get_relative_path,$(file)))
OBJS = $(REL_SRCS:.c=.o)

REL_ASM_SRCS = $(foreach file, $(ASM_SRCS),  $(OBJECTS_DIR)/$(call get_relative_path,$(file)))
ASM_OBJS = $(REL_ASM_SRCS:.S=.o)

#ALL directories containing a .h file
EXTRA_INC_PATHS += $(sort $(foreach dir, $(INCS),$(dir $(dir))))
FLAGS_WITHOUT_D = $(sort $(subst -D,,$(filter -D%, $(CFLAGS))))

CFLAGS += $(addprefix -I,$(EXTRA_INC_PATHS))

#------------------------------------------------------------------------------
#                             Generic Goals                         
#------------------------------------------------------------------------------

FILES = $(SRCS) $(INCS)

.DEFAULT_GOAL := all
PHONY += all
ifneq ($(wildcard $(BUILD_DIR)),)
all: $(BINARY)
else
all:
	$(MAKE) project MAKEFLAGS=
	$(MAKE) update_srcs
	$(MAKE) $(BINARY)
endif

#This is used to keep directory targets between makefile executions
#More details: http://ismail.badawi.io/blog/2017/03/28/automatic-directory-creation-in-make/
#Also the last . is explained
.PRECIOUS: $(OBJECTS_DIR)/. $(OBJECTS_DIR)%/.

$(OBJECTS_DIR)/.:
	-@$(call mk_dir,$@)

$(OBJECTS_DIR)%/.:
	-@$(call mk_dir,$@)

.SECONDEXPANSION:
$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).c | $$(@D)/.
	@echo CC -c $(notdir $<) -o $(notdir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).S | $$(@D)/.
	@echo AS -c $(notdir $<) -o $(notdir $@)
	@$(AS) -c $(ASFLAGS) $< -o $@

$(BINARY): $(LIB_TARGETS) $(OBJS) $(ASM_OBJS) $(LSCRIPT)
	@echo CC LDFLAGS -o $(BINARY)
	@$(CC) -T$(LSCRIPT) $(LDFLAGS) $(LIB_PATHS) -o $(BINARY) $(OBJS) $(ASM_OBJS) $(LIB_FLAGS) 

run: $(PLATFORM)_run

#This rule shouldn't be called with -j flag
project: $(PLATFORM)_project

project_build: $(PLATFORM)_project_build

# Remove project binaries
PHONY += clean
clean:
	-$(call remove_fun,$(BINARY))
	-$(call remove_fun,$(OBJS))

# Remove workspace data and project directory
PHONY += clean_all
clean_all: clean_libs
	-$(call remove_dir,$(BUILD_DIR))

# Rebuild porject
PHONY += re
re: clean
	@$(MAKE) all

PHONY += ra
ra: clean_all
	@$(MAKE) all

PHONY += list
list:
ifeq ($(OS), Windows_NT)
	@echo $(subst $(space),,$(sort $(SRCS) $(INCS)))
else
	@echo $(sort $(SRCS) $(INCS)) | tr ' ' '\n'
endif

.PHONY: $(PHONY)