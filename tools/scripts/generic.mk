export CFLAGS
export CC
export AR
export EXTRA_INC_PATHS
export FLAGS_WITHOUT_D
export PROJECT_BUILD
export JTAG_CABLE_ID
export BOOTOBJ

ifeq ($(VERBOSE),y)
else
HIDE = > /dev/zero
.SILENT:
endif

TIMESTAMP = $(shell date +"%T")
copy_file = cp $1 $2
copy_dir = cp -r $1 $2
remove_file = rm -rf $1
remove_dir_action = rm -rf $1
remove_dir = rm -rf $1
mk_dir = mkdir -p $1
read_file = cat $1 2>/dev/zero
print_lines = echo $1 | tr ' ' '\n'
green = \\e[32m$1\\e[39m
print = printf "$(call green,[$(TIMESTAMP)]) $1\n"
make_dir_link = ln -sf $1 $2
make_link = ln -sf $1 $2

# Recursive wildcard
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Convert full path to relative path
# $(PROJECT)/something <-> srcs/something
# $(NO-OS)/something <-> noos/something
RELATIVE_PATH = $(patsubst $(NO-OS)%,noos%,$(patsubst $(PROJECT)%,$(PROJECT_NAME)%,$1))
FULL_PATH = $(patsubst noos%,$(NO-OS)%,$(patsubst $(PROJECT_NAME)%,$(PROJECT)%,$1))
get_relative_path = $(patsubst $(ROOT_DRIVE)%,root/%,$(RELATIVE_PATH))
get_full_path = $(patsubst root/%,$(ROOT_DRIVE)%,$(FULL_PATH))

ifeq 'y' '$(strip $(LINK_SRCS))'
update_file = $(make_link)
update_dir = $(make_dir_link)
ACTION = Linking
else
update_file = $(call copy_file,$1,$(dir $2))
update_dir = $(copy_dir)
ACTION = Copying
endif

# Display the platform for which build is launched
print_build_type = $(call print,Building for $(call green,$1))

ifndef NO-OS
$(error "NO-OS variable needs to be set")
endif

# Need in libraries
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

# USED IN MAKEFILE
PROJECT_NAME	= $(notdir $(PROJECT))
OBJECTS_DIR		= $(BUILD_DIR)/objs
PLATFORM_TOOLS	= $(NO-OS)/tools/scripts/platform/$(PLATFORM)
BINARY_FILE_NAME ?= $(PROJECT_NAME)
BINARY			?=  $(BUILD_DIR)/$(BINARY_FILE_NAME).elf
BUILD_LOCK = $(BUILD_DIR)/.project.target
VSCODE_CFG_DIR	= $(PROJECT)/.vscode
VSCODE_SUPPORT	?= no

# New line variable
define ENDL


endef

ifneq ($(words $(NO-OS)), 1)
$(error $(ENDL)ERROR:$(ENDL)\
Plese clone no-os in a path without spaces,$(ENDL)\
otherwise the makefile will not work well.$(ENDL)\
Current path is: $(NO-OS)$(ENDL))
endif

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
HARDWARE := $(filter %.ioc, $(HARDWARE))
ifeq '' '$(HARDWARE)'
$(error 'No HARDWARE for stm32 found. Add .ioc file.')
endif
include $(NO-OS)/tools/scripts/stm32.mk
endif

ifeq 'linux' '$(PLATFORM)'
include $(NO-OS)/tools/scripts/linux.mk
endif

ifeq 'maxim' '$(PLATFORM)'
include $(NO-OS)/tools/scripts/maxim.mk
endif

ifeq 'pico' '$(PLATFORM)'
include $(NO-OS)/tools/scripts/pico.mk
endif

ifeq 'mbed' '$(PLATFORM)'
include $(NO-OS)/tools/scripts/mbed.mk
endif

#------------------------------------------------------------------------------
#                            COMMON COMPILER FLAGS                             
#------------------------------------------------------------------------------
CFLAGS += $(NEW_CFLAGS)
CPPFLAGS += $(NEW_CFLAGS)
CFLAGS += -Wall								\
	 -Wextra							\
	 -Wno-unused-parameter						\
	 -MMD 								\
	 -MP								\
	 -lm

#------------------------------------------------------------------------------
#                          COMMON INITIALIZATION
#------------------------------------------------------------------------------
relative_to_project = $(addprefix $(PROJECT_BUILD)/,$(call get_relative_path,$1))

include $(NO-OS)/tools/scripts/libraries.mk

ifeq (y,$(strip $(RELEASE)))
CFLAGS += -O2
endif

ifeq (y,$(strip $(NETWORKING)))
CFLAGS += -DNO_OS_NETWORKING
endif

ifeq (y,$(strip $(DISABLE_SECURE_SOCKET)))
CFLAGS += -DDISABLE_SECURE_SOCKET
endif

SRC_DIRS := $(patsubst %/,%,$(SRC_DIRS))

# Get all .c, .cpp and .h files from SRC_DIRS
SRCS     += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.c))
SRCS     += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.cpp))
ASM_SRCS += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.S))
ASM_SRCS += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.s))
INCS     += $(foreach dir, $(SRC_DIRS), $(call rwildcard, $(dir),*.h))

# Recursive ignored files. If a directory is in the variable IGNORED_FILES,
# all files from inside the directory will be ignored
ALL_IGNORED_FILES += $(foreach dir, $(IGNORED_FILES), $(call rwildcard, $(dir),*))

# Remove ignored files
SRCS     := $(filter-out $(ALL_IGNORED_FILES),$(SRCS))
INCS     := $(filter-out $(ALL_IGNORED_FILES),$(INCS))
ASM_SRCS := $(filter-out $(ALL_IGNORED_FILES),$(ASM_SRCS))

# Get all src files that are not in SRC_DRIS
FILES_OUT_OF_DIRS := $(filter-out $(foreach source_directory_name,$(sort $(SRC_DIRS)),$(wildcard $(source_directory_name)/*)),$(SRCS) $(INCS) $(ASM_SRCS))

REL_SRCS = $(addprefix $(OBJECTS_DIR)/,$(call get_relative_path,$(SRCS_IN_BUILD) $(PLATFORM_SRCS)))
OBJS = $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(REL_SRCS)))

REL_ASM_SRCS = $(addprefix $(OBJECTS_DIR)/,$(call get_relative_path,$(ASM_SRCS)))
ASM_OBJS_s = $(REL_ASM_SRCS:.s=.o)
ifneq ($(REL_ASM_SRCS),$(ASM_OBJS_s))
	ASM_OBJS += $(ASM_OBJS_s)
endif
ASM_OBJS_S = $(REL_ASM_SRCS:.S=.o)
ifneq ($(REL_ASM_SRCS),$(ASM_OBJS_S))
	ASM_OBJS += $(ASM_OBJS_S)
endif

# Will be used to add these flags to sdk project
FLAGS_WITHOUT_D = $(sort $(subst -D,,$(filter -D%, $(CFLAGS))))

# Remove duplicates
SRCS := $(sort $(SRCS))
ASM_SRCS := $(sort $(ASM_SRCS))
INCS := $(sort $(INCS))

CREATED_DIRECTORIES += noos root $(PROJECT_NAME)
SRCS_IN_BUILD = $(call relative_to_project, $(SRCS))
INCS_IN_BUILD = $(call relative_to_project, $(INCS))
DIRS_TO_CREATE = $(sort $(call relative_to_project, $(dir $(FILES_OUT_OF_DIRS)) $(SRC_DIRS) $(PLATFORM_DIRS)))
# Prefixes from get_relative_path
DIRS_TO_REMOVE = $(addprefix $(PROJECT_BUILD)/,$(CREATED_DIRECTORIES))

# Add to include all directories containing a .h file
EXTRA_INC_PATHS := $(sort $(foreach dir, $(INCS_IN_BUILD),$(dir $(dir))))
EXTRA_INC_PATHS := $(filter-out $(call relative_to_project,$(NO-OS)/include/),$(EXTRA_INC_PATHS))
EXTRA_INC_PATHS += $(call relative_to_project, $(INCLUDE))

CFLAGS += $(addprefix -I,$(EXTRA_INC_PATHS)) $(PLATFORM_INCS)
CPPFLAGS += $(addprefix -I,$(EXTRA_INC_PATHS)) $(PLATFORM_INCS)

# Text files containing build pre-requisite names.
OBJS_FILE = $(BUILD_DIR)/$(PROJECT_NAME)-objs.txt
CFLAGS_FILE = $(BUILD_DIR)/$(PROJECT_NAME)-cflags.txt
CPPFLAGS_FILE = $(BUILD_DIR)/$(PROJECT_NAME)-cppflags.txt
ASFLAGS_FILE = $(BUILD_DIR)/$(PROJECT_NAME)-asflags.txt

# Prepare for VS Code Debug Intellisense applied at "project" - depends on a complete CFLAGS
include $(NO-OS)/tools/scripts/vsc_intellisense.mk
# Prepare for VS Code Debug config applied at target "project"
include $(NO-OS)/tools/scripts/vsc_openocd_debug.mk
# Prepare for VS Code Tasks config applied at target $(PROJECT_TARGET)_configure
include $(NO-OS)/tools/scripts/vsc_tasks.mk

#------------------------------------------------------------------------------
#                             Generic Goals                         
#------------------------------------------------------------------------------

.DEFAULT_GOAL := all
PHONY += all
ifneq ($(wildcard $(BUILD_LOCK)),)
all:
	$(call print_build_type,$(PLATFORM))
	$(MAKE) --no-print-directory build
	$(call print,Done ($(notdir $(BUILD_DIR))/$(notdir $(BINARY))))
else
all:
	$(call print_build_type,$(PLATFORM))
	$(MAKE) --no-print-directory project
	$(MAKE) --no-print-directory update
	$(MAKE) --no-print-directory build
	$(call print,Done ($(notdir $(BUILD_DIR))/$(notdir $(BINARY))))
endif

# This is used to keep directory targets between makefile executions
# More details: http://ismail.badawi.io/blog/2017/03/28/automatic-directory-creation-in-make/
# Also the last . is explained
.PRECIOUS: $(OBJECTS_DIR)/. $(OBJECTS_DIR)%/.

# Create directories for binary files. This is needed in order to know from which
# .c it was build
$(OBJECTS_DIR)/.:
	-$(call mk_dir,$@)

$(OBJECTS_DIR)%/.:
	-$(call mk_dir,$@)

# Build .c files into .o files.
.SECONDEXPANSION:
$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).c | $$(@D)/.
	$(call print,[CC] $(notdir $<))
	$(CC) -c @$(CFLAGS_FILE) $< -o $@

$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).cpp | $$(@D)/.
	$(call print,[CPP] $(notdir $<))
	$(CPP) -c @$(CPPFLAGS_FILE) $< -o $@

$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).s | $$(@D)/. 
	$(call print,[AS] $(notdir $<))
	$(AS) -c @$(ASFLAGS_FILE) $< -o $@

$(OBJECTS_DIR)/%.o: $$(call get_full_path, %).S | $$(@D)/. 
	$(call print,[AS] $(notdir $<))
	$(AS) -c @$(ASFLAGS_FILE) $< -o $@

ifneq ($(strip $(LSCRIPT)),)
LSCRIPT_FLAG = -T$(LSCRIPT)
endif

define generate_cflags_func
echo $(subst \",\\\",$(1)) >> $(CFLAGS_FILE)
endef

define generate_cppflags_func
echo $(subst \",\\\",$(1)) >> $(CPPFLAGS_FILE)
endef

define generate_asflags_func
echo $(subst \",\\\",$(1)) >> $(ASFLAGS_FILE)
endef

define generate_objs_func
echo $(1) >> $(OBJS_FILE)
endef

PHONY += pre_build
pre_build:
	$(call print,Generating build flags)
	echo -n > $(CFLAGS_FILE)
	$(call process_items_in_chunks,$(CFLAGS),10,generate_cflags_func)
	echo -n > $(CPPFLAGS_FILE)
	$(call process_items_in_chunks,$(CPPFLAGS),10,generate_cppflags_func)
	echo -n > $(ASFLAGS_FILE)
	$(call process_items_in_chunks,$(ASFLAGS),10,generate_asflags_func)
	echo -n > $(OBJS_FILE)
	$(call process_items_in_chunks,$(sort $(OBJS)),10,generate_objs_func)

$(BINARY): $(LIB_TARGETS) $(OBJS) $(ASM_OBJS) $(LSCRIPT) $(BOOTOBJ)
	$(call print,[LD] $(notdir $(OBJS)))
	$(CC) $(LSCRIPT_FLAG) $(LDFLAGS) $(LIB_PATHS) -o $(BINARY) @$(OBJS_FILE) $(EXTRA_FILES) $(BOOTOBJ)\
			 $(ASM_OBJS) $(LIB_FLAGS)
	$(MAKE) --no-print-directory post_build

PHONY += $(BINARY).id
$(BINARY).id:
	@echo interface cmsis-dap > $(BINARY).id
ifneq ($(JTAG_CABLE_ID),)
	@echo cmsis_dap_serial $(JTAG_CABLE_ID) >> $(BINARY).id
endif

PHONY += run
run: $(PLATFORM)_run
	$(call print,$(notdir $(BINARY)) uploaded to board)

project: $(LIB_TARGETS) $(PLATFORM)_project
	@echo "Build lock." > $(BUILD_LOCK)

# Platform specific post build dependencies can be added to this rule.
post_build: $(PLATFORM)_post_build
	$(SIZE) --format=Berkley $(BINARY) $(HEX)

# Function to process a list in chunks
# Arguments:
#   $(1) - List of items
#   $(2) - Chunk size
#   $(3) - Action to be performed on each item
define process_items_in_chunks
	$(eval ITEMS := $(wordlist 1,$(2),$(1)))
	$(if $(ITEMS), \
		$(foreach ITEM,$(ITEMS),$(call $(3),$(ITEM));) \
		$(eval REMAINING := $(subst $(ITEMS),,$(1))) \
		$(call process_items_in_chunks,$(REMAINING),$(2),$(3)) \
	)
endef

define update_file_func
$(call update_file,$(1),$(call relative_to_project,$(1)))
endef

PHONY += update
update:
	$(call print,$(ACTION) srcs to created project)
	$(call remove_dir,$(DIRS_TO_REMOVE))
	$(call mk_dir,$(DIRS_TO_CREATE))
	$(call process_items_in_chunks,$(sort $(SRCS) $(INCS) $(ASM_SRCS)),10,update_file_func)
	$(MAKE) --no-print-directory pre_build

standalone:
	$(MAKE) --no-print-directory project LINK_SRCS=n

# Build project using generic compiler
build: $(BINARY)

# Build project using SDK
sdkbuild: $(PLATFORM)_sdkbuild

# Remove build artefacts created using the sdkbuild rule
sdkclean: $(PLATFORM)_sdkclean

# Open the project using the corresponding SDK
sdkopen: $(PLATFORM)_sdkopen

# Remove build artefacts
PHONY += clean
clean:
	$(call print,[Delete] $(notdir $(OBJS) $(BINARY) $(ASM_OBJS)))
	$(call remove_file,$(BINARY) $(OBJS) $(ASM_OBJS))

# Remove the whole build directory
PHONY += reset
reset: clean_libs $(PLATFORM)_reset
	$(call print,[Delete] $(BUILD_DIR))
	$(call remove_dir,$(BUILD_DIR))

PHONY += list
list:
	$(call print_lines, $(sort $(SRCS) $(INCS) $(ASM_SRCS)))

.PHONY: $(PHONY)

# Adding header files dependancy on object files
-include $(OBJS:.o=.d)
