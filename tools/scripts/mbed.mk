# For Mbed Platform SDP_K1 is default but can be changed by giving specific Mbed supported board name to TARGET_BOARD variable while giving make command
TARGET_BOARD = SDP_K1
COMPILER = GCC_ARM

# Build directory for Mbed Platform
PROJECT_BUILD = $(BUILD_DIR)

# Mbed-OS related paths and files
MBED_OS_DIRECTORY = $(NO-OS)/libraries/mbed
MBED_OS_BUILD_DIRECTORY = $(BUILD_DIR)/BUILD/$(TARGET_BOARD)/$(COMPILER)
MBED_OS_LIBRARY = $(MBED_OS_BUILD_DIRECTORY)/libmbed-os.a
MBED_APP_JSON_DIRECTORY = $(MBED_OS_DIRECTORY)/mbed-app-json

# Finding linker-script in Mbed-OS build directory
LINKER_SCRIPT_BEFORE_PREPROCESSING = $(sort $(call rwildcard,$(MBED_OS_BUILD_DIRECTORY),*.ld))

# compiler name and standard libraries
LIB_FLAGS = -Wl,--start-group -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys  -Wl,--end-group
CC = arm-none-eabi-gcc
CPP = arm-none-eabi-g++
OC = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Project related build Files
LSCRIPT = $(BUILD_DIR)/$(PROJECT_NAME)-linker-file.ld
PROJECT_BIN_FILE = $(subst elf,bin,$(BINARY))
PROJECT_MAP_FILE = $(subst elf,map,$(BINARY))

# Reading Flags and Object Files and Include files for mbed-os
MBED_GENERATED_ARCHIVE_FILE = $(MBED_OS_BUILD_DIRECTORY)/.archive_files.txt
UPDATED_MBED_GENERATED_ARCHIVE_FILE = $(MBED_OS_BUILD_DIRECTORY)/.updated_archive_files.txt
READ_MBED_GENERATED_PROFILE_C_FILE = $(shell $(call read_file ,$(MBED_OS_BUILD_DIRECTORY)/.profile-c))
READ_MBED_GENERATED_PROFILE_CPP_FILE = $(shell $(call read_file ,$(MBED_OS_BUILD_DIRECTORY)/.profile-cxx))
READ_MBED_GENERATED_PROFILE_ASM_FILE = $(shell $(call read_file ,$(MBED_OS_BUILD_DIRECTORY)/.profile-asm))
READ_MBED_GENERATED_PROFILE_LINKER_FILE = $(shell $(call read_file ,$(MBED_OS_BUILD_DIRECTORY)/.profile-ld))
READ_MBED_GENERATED_INCLUDE_FILE = $(shell $(call read_file ,$(MBED_OS_BUILD_DIRECTORY)/.includes_*))

NULL := 
COMMA :=,
SPACE := $(NULL) $(NULL)

MBED_C_FLAGS = $(subst ",,$(subst $(COMMA), ,$(patsubst {"flags":[%,%,$(firstword $(subst ]$(COMMA)"macros", "macros",$(subst $(SPACE),,$(READ_MBED_GENERATED_PROFILE_C_FILE)))))))
MBED_C_SYMBOLS = $(patsubst %,-D%,$(subst ",,$(subst $(COMMA), ,$(patsubst %]},%,$(lastword $(subst "symbols":[,"symbols":[ ,$(subst $(SPACE),,$(READ_MBED_GENERATED_PROFILE_C_FILE))))))))
CFLAGS += $(MBED_C_FLAGS) $(MBED_C_SYMBOLS) -include $(MBED_OS_BUILD_DIRECTORY)/mbed_config.h -DMBED_PLATFORM

MBED_CPP_FLAGS = $(subst ",,$(subst $(COMMA), ,$(patsubst {"flags":[%,%,$(firstword $(subst ]$(COMMA)"macros", "macros",$(subst $(SPACE),,$(READ_MBED_GENERATED_PROFILE_CPP_FILE)))))))
MBED_CPP_SYMBOLS = $(patsubst %,-D%,$(subst ",,$(subst $(COMMA), ,$(patsubst %]},%,$(lastword $(subst "symbols":[,"symbols":[ ,$(subst $(SPACE),,$(READ_MBED_GENERATED_PROFILE_CPP_FILE))))))))
CPPFLAGS += $(MBED_CPP_FLAGS) $(MBED_CPP_SYMBOLS) -include $(MBED_OS_BUILD_DIRECTORY)/mbed_config.h -DMBED_PLATFORM

MBED_ASM_FLAGS = $(subst ",,$(subst $(COMMA), ,$(patsubst {"flags":[%,%,$(firstword $(subst ]$(COMMA)"macros", "macros",$(subst $(SPACE),,$(READ_MBED_GENERATED_PROFILE_ASM_FILE)))))))
MBED_ASM_SYMBOLS = $(patsubst %,-D%,$(subst ",,$(subst $(COMMA), ,$(patsubst %]},%,$(lastword $(subst "symbols":[,"symbols":[ ,$(subst $(SPACE),,$(READ_MBED_GENERATED_PROFILE_ASM_FILE))))))))
ASFLAGS += $(MBED_ASM_FLAGS) $(MBED_ASM_SYMBOLS) -include $(MBED_OS_BUILD_DIRECTORY)/mbed_config.h -DMBED_PLATFORM

MBED_LINKER_FLAGS = $(subst ",,$(subst "$(COMMA)", ,$(patsubst {"flags":[%,%,$(firstword $(subst ]$(COMMA)"macros", "macros",$(subst $(SPACE),,$(READ_MBED_GENERATED_PROFILE_LINKER_FILE)))))))
MBED_LINKER_SYMBOLS = $(patsubst %,-D%,$(subst ",,$(subst $(COMMA), ,$(patsubst %]},%,$(lastword $(subst "symbols":[,"symbols":[ ,$(subst $(SPACE),,$(READ_MBED_GENERATED_PROFILE_LINKER_FILE))))))))
LDFLAGS += $(MBED_LINKER_FLAGS)

# Preprocessor flags for linker Script
MBED_PREPROCESSOR_FLAGS = arm-none-eabi-cpp -E -P $(LDFLAGS)

# Map file compiler flag
LDFLAGS += $(MBED_LINKER_SYMBOLS) -Wl,-Map=$(PROJECT_MAP_FILE)

# Updating mbed generated archive file
UPDATE_MBED_GENERATED_ARCHIVE_FILE = $(addprefix $(MBED_OS_DIRECTORY)/, $(shell $(call read_file ,$(MBED_GENERATED_ARCHIVE_FILE))))

# Platform include paths
MBED_PLATFORM_INCLUDE_PATHS = $(patsubst %,%/,$(patsubst %,-I%,$(patsubst mbed-os%,$(MBED_OS_DIRECTORY)/mbed-os%,$(patsubst -I%,%,$(subst ",,$(READ_MBED_GENERATED_INCLUDE_FILE))))))
PLATFORM_INCS = $(MBED_PLATFORM_INCLUDE_PATHS)

# Extra files for build
EXTRA_FILES =@$(UPDATED_MBED_GENERATED_ARCHIVE_FILE)

define generate_obj_func
echo $(1) >> $(UPDATED_MBED_GENERATED_ARCHIVE_FILE)
endef

# Rule for building Mbed-OS
$(PLATFORM)_project: MBED-OS-build
	-$(call mk_dir,$(BUILD_DIR)) $(HIDE)
	$(call print, putting mbed-os object files names to text file)
	echo -n > $(UPDATED_MBED_GENERATED_ARCHIVE_FILE)
	$(call process_items_in_chunks,$(sort $(UPDATE_MBED_GENERATED_ARCHIVE_FILE)),10,generate_obj_func)

$(MBED_OS_LIBRARY):
	$(MAKE) --no-print-directory MBED-OS-build $(HIDE)

PHONY_TARGET += MBED-OS-build 
MBED-OS-build:
	-$(call mk_dir,$(MBED_APP_JSON_DIRECTORY)) $(HIDE)
	$(call copy_file,$(MBED_OS_DIRECTORY)/mbed_app.json,$(MBED_APP_JSON_DIRECTORY)/) $(HIDE)
	cd $(MBED_OS_DIRECTORY); mbed config root .; mbed compile --source $(MBED_OS_DIRECTORY)/mbed-os --source $(MBED_APP_JSON_DIRECTORY) -m $(TARGET_BOARD) -t $(COMPILER) --build $(MBED_OS_BUILD_DIRECTORY) --library
	$(call print, Mbed-OS build completed)

# Linker-Script Preprocessing
$(LSCRIPT): $(LINKER_SCRIPT_BEFORE_PREPROCESSING)
	$(MBED_PREPROCESSOR_FLAGS) $< -o $@

# Binary file creation from elf file
$(PROJECT_BIN_FILE):$(BINARY)
	$(OC) -O binary $< $@
	$(call print,Done $(PROJECT_BIN_FILE))

$(PLATFORM)_post_build: $(PROJECT_BIN_FILE)

$(PLATFORM)_reset:
