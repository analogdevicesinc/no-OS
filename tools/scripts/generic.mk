#------------------------------------------------------------------------------
#                             EXPORTED VARIABLES                               
#------------------------------------------------------------------------------
# Used by nested Makefils (mbedtls, fatfs, iio)
export CFLAGS
export CC
export AR

# Used by the build utils
export EXTRA_INC_PATHS
export FLAGS_WITHOUT_D
export PROJECT_BUILD
#------------------------------------------------------------------------------
#                              UTIL FUNCTIONS
#------------------------------------------------------------------------------

#	WINDOWS
ifeq ($(OS), Windows_NT)
SHELL=cmd
ROOT_DRIVE = C:
#Is slow to print timestamp in windows
#TIMESTAMP = $(shell powershell Get-Date -Format "HH:mm.ss")
TIMESTAMP = 00:00:00
copy_fun = xcopy /F /Y /B "$(subst /,\,$1)" "$(subst /,\,$2)"
copy_folder = xcopy /F /S /Y /C /I "$(subst /,\,$1)" "$(subst /,\,$2)"
remove_fun = del /S /Q $(subst /,\,$1)
remove_dir = rd /S /Q $(addsuffix ",$(addprefix ",$(subst /,\,$1)))
mk_dir = md $(subst /,\,$1)
read_file = type $(subst /,\,$1) 2> NUL
make_dir_link = mklink /D "$(strip $(subst /,\,$2))" "$(strip $(subst /,\,$1))"
make_link = mklink "$(strip $(subst /,\,$2))" "$(strip $(subst /,\,$1))"
print_lines = $(foreach f,$1,@echo $f && ) @echo Done
green = [32m$1[0m
print = @echo $(call green,[$(TIMESTAMP)]) $1
cmd_separator = &
HIDE_OUTPUT = > nul

#	LINUX
else
TIMESTAMP = $(shell date +"%T")
copy_fun = cp $1 $2
copy_folder = cp -r $1 $2
remove_fun = rm -rf $1
remove_dir = rm -rf $1
mk_dir = mkdir -p $1
read_file = cat $1 2> /dev/null
make_dir_link = ln -s $1 $2
make_link = ln -s $1 $2
print_lines = @echo $1 | tr ' ' '\n'
green = \\e[32m$1\\e[39m
print = @printf "$(call green,[$(TIMESTAMP)]) $1\n"
cmd_separator = ;
HIDE_OUTPUT = > /dev/null
endif

VERBOSE ?= 0
export VERBOSE

ifeq ($(strip $(VERBOSE)),0)
HIDE = $(HIDE_OUTPUT)
MUTE = @
endif

ifeq ($(strip $(VERBOSE)),2)
HIDE = $(HIDE_OUTPUT)
endif

# recursive wildcard
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

#Creates file with the specified name
set_one_time_rule = echo Target file. Do not delete > $1

# Transforme full path to relative path to be used in build
# $(PROJECT)/something -> srcs/something
# $(NO-OS)/something -> noos/something
# $(PLATFORM_TOOLS)/something -> aducm3029/something TODO test without these
RELATIVE_PATH = $(patsubst $(NO-OS)%,noos%,$(patsubst $(PROJECT)%,$(PROJECT_NAME)%,$(PLATFORM_RELATIVE_PATH)))

# Transforme relative path to full path in order to find the needed .c files
# Reverse of get_relative_path
FULL_PATH = $(patsubst noos%,$(NO-OS)%,$(patsubst $(PROJECT_NAME)%,$(PROJECT)%,$(PLATFORM_FULL_PATH)))

ifeq ($(OS), Windows_NT)
get_relative_path = $(patsubst $(ROOT_DRIVE)%,root%,$(RELATIVE_PATH))
get_full_path = $(patsubst root%,$(ROOT_DRIVE)%,$(FULL_PATH))
else
get_relative_path = $(RELATIVE_PATH)
get_full_path = $(FULL_PATH)
endif

relative_to_project = $(addprefix $(PROJECT_BUILD)/,$(call get_relative_path,$1))

ifeq 'y' '$(strip $(LINK_SRCS))'
file_fun = $(make_link)
folder_fun = $(make_dir_link)
ACTION = Linking
else
file_fun = $(call copy_fun,$1,$(dir $2))
folder_fun = $(copy_folder)
ACTION = Copying
endif

#------------------------------------------------------------------------------
#                           ENVIRONMENT VARIABLES                              
#------------------------------------------------------------------------------

ifndef NO-OS
$(error "NO-OS variable needs to be set")
endif

#Need in libraries
export NO-OS

ifndef PROJECT
$(error "PROJECT variable needs to be set")
endif
ifndef BUILD_DIR
$(error "BUILD_DIR variable needs to be set")
endif
ifndef WORKSPACE
$(error "WORKSPACE variable needs to be set")
endif

#USED IN MAKEFILE
PROJECT_NAME	= $(notdir $(PROJECT))
PROJECT_BUILD 		= $(BUILD_DIR)/app
OBJECTS_DIR		= $(BUILD_DIR)/objs
PLATFORM_TOOLS	= $(NO-OS)/tools/scripts/platform/$(PLATFORM)
BINARY			?= $(BUILD_DIR)/$(PROJECT_NAME).elf
PROJECT_TARGET		= $(BUILD_DIR)/.project.target

ifneq ($(words $(NO-OS)), 1)
$(error $(ENDL)ERROR:$(ENDL)\
Plese clone no-os in a path without spaces,$(ENDL)\
otherwise the makefile will not work well.$(ENDL)\
Current path is: $(NO-OS)$(ENDL))
endif

#------------------------------------------------------------------------------
#                          INCLUDE SPECIFIC MAKEFILES
#------------------------------------------------------------------------------

ifeq 'xilinx' '$(PLATFORM)'
TMP_HW := $(filter %.xsa, $(HARDWARE))
ifeq '' '$(TMP_HW)'
TMP_HW := $(filter %.hdf, $(HARDWARE))
endif
HARDWARE := $(TMP_HW)
ifeq '' '$(HARDWARE)'
$(error 'No HARDWARE for xilinx found.')
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

ifeq 'stm32' '$(PLATFORM)'
include $(NO-OS)/tools/scripts/stm32.mk
endif

ifeq 'linux' '$(PLATFORM)'
include $(NO-OS)/tools/scripts/linux.mk
endif

#------------------------------------------------------------------------------
#                            COMMON COMPILER FLAGS                             
#------------------------------------------------------------------------------
CFLAGS += $(NEW_CFLAGS)
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

SRC_DIRS := $(patsubst %/,%,$(SRC_DIRS))

# Get all .c and .h files from SRC_DIRS
SRCS     += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.c))
INCS     += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.h))

# Recursive ignored files. If a directory is in the variable IGNORED_FILES,
# all files from inside the directory will be ignored
ALL_IGNORED_FILES = $(foreach dir, $(IGNORED_FILES), $(call rwildcard, $(dir),*))

# Remove ignored files
SRCS     := $(filter-out $(ALL_IGNORED_FILES),$(SRCS))
INCS     := $(filter-out $(ALL_IGNORED_FILES),$(INCS))

#Get all src files that are not in SRC_DRIS
FILES_OUT_OF_DIRS := $(filter-out $(call rwildcard, $(SRC_DIRS),*), $(SRCS) $(INCS)) 

REL_SRCS = $(addprefix $(OBJECTS_DIR)/,$(call get_relative_path,$(SRCS_IN_BUILD) $(PLATFORM_SRCS)))
OBJS = $(REL_SRCS:.c=.o)

REL_ASM_SRCS = $(addprefix $(OBJECTS_DIR)/,$(call get_relative_path,$(ASM_SRCS)))
ASM_OBJS_s = $(REL_ASM_SRCS:.s=.o)
ifneq ($(REL_ASM_SRCS),$(ASM_OBJS_s))
	ASM_OBJS += $(ASM_OBJS_s)
endif
ASM_OBJS_S = $(REL_ASM_SRCS:.S=.o)
ifneq ($(REL_ASM_SRCS),$(ASM_OBJS_S))
	ASM_OBJS += $(ASM_OBJS_S)
endif

#Will be used to add this flags to sdk project
FLAGS_WITHOUT_D = $(sort $(subst -D,,$(filter -D%, $(CFLAGS))))

#Remove duplicates
SRCS := $(sort $(SRCS))
INCS := $(sort $(INCS))

CREATED_DIRECTORIES += noos root $(PROJECT_NAME)
SRCS_IN_BUILD = $(call relative_to_project, $(SRCS))
INCS_IN_BUILD = $(call relative_to_project, $(INCS))
DIRS_TO_CREATE = $(sort $(dir $(call relative_to_project, $(FILES_OUT_OF_DIRS) $(SRC_DIRS))))
#Prefixes from get_relative_path 
DIRS_TO_REMOVE = $(addprefix $(PROJECT_BUILD)/,$(CREATED_DIRECTORIES))

#Add to include all directories containing a .h file
EXTRA_INC_PATHS := $(sort $(foreach dir, $(INCS_IN_BUILD),$(dir $(dir))))
CFLAGS += $(addprefix -I,$(EXTRA_INC_PATHS)) $(PLATFORM_INCS)

#------------------------------------------------------------------------------
#                             Generic Goals                         
#------------------------------------------------------------------------------

.DEFAULT_GOAL := all
PHONY += all
# If the build dir was created just build the binary.
# else the project will be build first. This will allow to run make with -j .
ifneq ($(wildcard $(PROJECT_TARGET)),)
all: print_build_type $(BINARY)
	$(call print,Done ($(notdir $(BUILD_DIR))/$(notdir $(BINARY))))
else
all: print_build_type
#Remove -j flag for running project target. (It doesn't work on xilinx on this target)
	$(MUTE) $(MAKE) --no-print-directory update_srcs MAKEFLAGS=$(MAKEOVERRIDES)
	$(MUTE) $(MAKE) --no-print-directory $(BINARY)
	$(call print,Done ($(notdir $(BUILD_DIR))/$(notdir $(BINARY))))
endif

PHONY += print_build_type
print_build_type:
	$(call print,Building for $(call green,$(PLATFORM)))

#This is used to keep directory targets between makefile executions
#More details: http://ismail.badawi.io/blog/2017/03/28/automatic-directory-creation-in-make/
#Also the last . is explained
.PRECIOUS: $(OBJECTS_DIR)/. $(OBJECTS_DIR)%/.

#Create directories for binary files. This is needed in order to know from which
# .c it was build
$(OBJECTS_DIR)/.:
	-@$(call mk_dir,$@) $(HIDE)

$(OBJECTS_DIR)%/.:
	-@$(call mk_dir,$@) $(HIDE)

# Build .c files into .o files.
.SECONDEXPANSION:
$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).c | $$(@D)/.
	@$(call print,[CC] $(notdir $<))
	$(MUTE) $(CC) -c $(CFLAGS) $< -o $@

$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).s | $$(@D)/. 
	@$(call print,[AS] $(notdir $<))
	$(MUTE) $(AS) -c $(ASFLAGS) $< -o $@

$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).S | $$(@D)/. 
	@$(call print,[AS] $(notdir $<))
	$(MUTE) $(AS) -c $(ASFLAGS) $< -o $@

ifneq ($(strip $(LSCRIPT)),)
LSCRIPT_FLAG = -T$(LSCRIPT) $(LDFLAGS)
endif

$(BINARY): $(LIB_TARGETS) $(OBJS) $(ASM_OBJS) $(LSCRIPT)
	@$(call print,[LD] $(notdir $(OBJS)))
	$(MUTE) $(CC) $(LSCRIPT_FLAG) $(LDFLAGS) $(LIB_PATHS) -o $(BINARY) $(OBJS) \
			 $(ASM_OBJS) $(LIB_FLAGS)
	$(MUTE) $(MAKE) --no-print-directory post_build

PHONY += run
run: $(PLATFORM)_run
	@$(call print,$(notdir $(BINARY)) uploaded to board)

project: $(PROJECT_TARGET)

$(PROJECT_TARGET): $(LIB_TARGETS)

#Platform specific post build dependencies can be added to this rule.
post_build:

PHONY += update_srcs
update_srcs: $(PROJECT_TARGET)
	@$(call print, $(ACTION) srcs to created project)
	-$(MUTE)$(call remove_dir,$(DIRS_TO_REMOVE)) $(HIDE)
	$(MUTE) -$(call mk_dir,$(DIRS_TO_CREATE)) $(HIDE)
	$(MUTE) $(foreach dir,$(sort $(SRC_DIRS)),\
		$(call folder_fun,$(dir),$(call relative_to_project,$(dir))) $(HIDE)\
		$(cmd_separator)) echo . $(HIDE)
	$(MUTE) $(foreach file,$(sort $(FILES_OUT_OF_DIRS)),\
		$(call file_fun,$(file),$(call relative_to_project,$(file))) $(HIDE)\
		$(cmd_separator)) echo . $(HIDE)

standalone:
	$(MUTE) $(MAKE) --no-print-directory project LINK_SRCS=n MAKEFLAGS=$(MAKEOVERRIDES)

# Build project using SDK
project_build: $(PLATFORM)_project_build

# Remove build artefacts
PHONY += clean
clean:
	$(call print,[Delete] $(notdir $(OBJS) $(BINARY) $(ASM_OBJS)))
	-$(MUTE)$(call remove_fun,$(BINARY) $(OBJS) $(ASM_OBJS)) $(HIDE)

# Remove the whole build directory
PHONY += clean_all
clean_all: clean_libs
	@$(call print,[Delete] $(BUILD_DIR))
	-$(MUTE)$(call remove_dir,$(BUILD_DIR)) $(HIDE)

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
