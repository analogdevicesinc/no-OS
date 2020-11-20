#By default for the moment old makefiles will be used
USE_OLD_MAKEFILE ?= y
USE_OLD_MAKEFILE = n
ifeq '$(strip $(USE_OLD_MAKEFILE))' 'y'

#------------------------------------------------------------------------------
#                              UTIL FUNCTIONS
#------------------------------------------------------------------------------
# recursive wildcard
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#------------------------------------------------------------------------------
#                          COMMON INITIALIZATION
#------------------------------------------------------------------------------

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

#------------------------------------------------------------------------------
#                          INCLUDE SPECIFIC MAKEFILES
#------------------------------------------------------------------------------

ifeq (aducm3029,$(strip $(PLATFORM)))
#Aducm3029 makefile
include $(NO-OS)/tools/scripts/aducm.mk

else
#Xilnx and altera makefiles
ifeq ($(OS), Windows_NT)
include $(NO-OS)/tools/scripts/windows.mk
else
include $(NO-OS)/tools/scripts/linux.mk
endif #($(OS), Windows_NT)

endif #(aducm3029,$(strip $(PLATFORM)))

list:
ifeq ($(OS), Windows_NT)
	@echo $(subst $(space),,$(sort $(SRCS) $(INCS)))
else
	@echo $(sort $(SRCS) $(INCS)) | tr ' ' '\n'
endif

else

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
ROOT_DRIVE = C:
copy_fun = copy /Y /B "$(subst /,\,$1)" "$(subst /,\,$2)"
copy_folder = xcopy /S /Y /C /I "$(subst /,\,$1)" "$(subst /,\,$2)"
remove_fun = del /S /Q $(subst /,\,$1)
remove_dir = rd /S /Q "$(subst /,\,$1)"
mk_dir = md $(subst /,\,$1)
read_file = type $(subst /,\,$1) 2> NUL
make_dir_link = mklink /D "$(subst /,\,$2)" "$(subst /,\,$1)"
make_link = mklink "$(subst /,\,$2)" "$(subst /,\,$1)"
print_lines = $(foreach f,$1,@echo $f && ) @echo Done
#	LINUX
else
copy_fun = cp $1 $2
copy_folder = cp -r $1 $2
remove_fun = rm -rf $1
remove_dir = rm -rf $1
mk_dir = mkdir -p $1
read_file = cat $1 2> /dev/null
make_dir_link = ln -s $1 $2
make_link = $(make_dir_link)
print_lines = @echo $1 | tr ' ' '\n'
endif

# recursive wildcard
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#Creates file with the specified name
set_one_time_rule = echo Target file. Do not delete > $1

test:
	$(call set_one_time_rule,$@)

# Transforme full path to relative path to be used in build
# $(PROJECT)/something -> srcs/something
# $(NO-OS)/something -> noos/something
# $(PLATFORM_TOOLS)/something -> aducm3029/something TODO test without these
RELATIVE_PATH = $(patsubst $(NO-OS)%,noos%,$(patsubst $(PROJECT)%,$(TARGET)%,$1))

# Transforme relative path to full path in order to find the needed .c files
# Reverse of get_relative_path
FULL_PATH = $(patsubst noos%,$(NO-OS)%,$(patsubst $(TARGET)%,$(PROJECT)%,$1))

ifeq ($(OS), Windows_NT)
get_relative_path = $(patsubst $(ROOT_DRIVE)%,root%,$(RELATIVE_PATH))
get_full_path = $(patsubst root%,$(ROOT_DRIVE)%,$(FULL_PATH))
else
get_relative_path = $(RELATIVE_PATH)
get_full_path = $(FULL_PATH)
endif



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
BINARY			= $(BUILD_DIR)/$(PROJECT_NAME)_release

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

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
CFLAGS += -DENABLE_IIO_NETWORK
endif

ifeq (y,$(strip $(DISABLE_SECURE_SOCKET)))
CFLAGS += -DDISABLE_SECURE_SOCKET
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

REL_SRCS = $(addprefix $(OBJECTS_DIR)/,$(call get_relative_path,$(SRCS)))
OBJS = $(REL_SRCS:.c=.o)

REL_ASM_SRCS = $(addprefix $(OBJECTS_DIR)/,$(call get_relative_path,$(ASM_SRCS)))
ASM_OBJS = $(REL_ASM_SRCS:.S=.o)

#Add to include all directories containing a .h file
EXTRA_INC_PATHS += $(sort $(foreach dir, $(INCS),$(dir $(dir))))
CFLAGS += $(addprefix -I,$(EXTRA_INC_PATHS))

#Will be used to add this flags to sdk project
FLAGS_WITHOUT_D = $(sort $(subst -D,,$(filter -D%, $(CFLAGS))))

#------------------------------------------------------------------------------
#                             Generic Goals                         
#------------------------------------------------------------------------------

.DEFAULT_GOAL := all
PHONY += all
# If the build dir was created just build the binary.
# else the project will be build first. This will allow to run make with -j .
ifneq ($(wildcard $(BUILD_DIR)),)
all: $(BINARY)
else
all:
#Remove -j flag for running project target. (It doesn't work on xilinx on this target)
	$(MAKE) project MAKEFLAGS=$(MAKEOVERRIDES)
	$(MAKE) $(BINARY)
endif

#This is used to keep directory targets between makefile executions
#More details: http://ismail.badawi.io/blog/2017/03/28/automatic-directory-creation-in-make/
#Also the last . is explained
.PRECIOUS: $(OBJECTS_DIR)/. $(OBJECTS_DIR)%/.

#Create directories for binary files. This is needed in order to know from which
# .c it was build
$(OBJECTS_DIR)/.:
	-@$(call mk_dir,$@)

$(OBJECTS_DIR)%/.:
	-@$(call mk_dir,$@)

# Build .c files into .o files.
.SECONDEXPANSION:
$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).c | $$(@D)/.
	@echo CC -c $(notdir $<) -o $(notdir $@)
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).S | $$(@D)/.
	@echo AS -c $(notdir $<) -o $(notdir $@)
	@$(AS) -c $(ASFLAGS) $< -o $@

$(BINARY): $(LIB_TARGETS) $(OBJS) $(ASM_OBJS) $(LSCRIPT)
	@echo CC LDFLAGS -o $(BINARY)
	@$(CC) -T$(LSCRIPT) $(LDFLAGS) $(LIB_PATHS) -o $(BINARY) $(OBJS) \
			 $(ASM_OBJS) $(LIB_FLAGS) 

PHONY += run
run: $(PLATFORM)_run

project: $(PLATFORM)_project

PHONY += update_srcs
update_srcs: $(PLATFORM)_update_srcs

# Build project using SDK
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

PHONY += ca
ca: clean_all

# Rebuild porject
PHONY += re
re: clean
	$(MAKE) all

PHONY += ra
ra: clean_all
	$(MAKE) all

PHONY += list
list:
	$(call print_lines, $(sort $(SRCS) $(INCS)))

.PHONY: $(PHONY)

endif #USE_OLD_MAKFILE
