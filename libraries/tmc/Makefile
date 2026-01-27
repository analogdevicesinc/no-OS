# TMC Library Makefile

# Build directory
BUILD_DIR ?= build
TMC_BUILD_DIR ?= $(BUILD_DIR)/tmc_libs

# Source directories
TMC_API_DIR = $(NO-OS)/libraries/tmc/TMC-API/
HELPERS_DIR = $(TMC_API_DIR)/tmc/helpers
IC_DIR = $(TMC_API_DIR)/tmc/ic

# Compiler and archiver settings

CFLAGS += -I$(TMC_API_DIR)

# Find all helper C files
HELPER_SRCS = $(wildcard $(HELPERS_DIR)/*.c)
HELPER_OBJS = $(patsubst $(HELPERS_DIR)/%.c,$(TMC_BUILD_DIR)/helpers/%.o,$(HELPER_SRCS))

# Find all TMC IC directories (TMC-API/tmc/ic/TMCXXXX)
TMC_DIRS = $(wildcard $(IC_DIR)/*)
TMC_LIBS = $(patsubst $(IC_DIR)/%,$(TMC_BUILD_DIR)/lib%.a,$(notdir $(TMC_DIRS)))

# Default target
.DEFAULT_GOAL := all
all: $(TMC_LIBS)

# Allow building individual TMC libraries by name (e.g., make TMC2130)
$(notdir $(TMC_DIRS)): %: $(TMC_BUILD_DIR)/lib%.a

# Create build directories
$(TMC_BUILD_DIR)/helpers:
	@if not exist "$(subst /,\,$@)" mkdir "$(subst /,\,$@)"

$(TMC_BUILD_DIR):
	@if not exist "$(subst /,\,$@)" mkdir "$(subst /,\,$@)"

# Build helper object files
$(TMC_BUILD_DIR)/helpers/%.o: $(HELPERS_DIR)/%.c
	@mkdir -p $(TMC_BUILD_DIR)/helpers
	$(CC) $(CFLAGS) -c $< -o $@

# Build TMC libraries
$(TMC_BUILD_DIR)/lib%.a: $(IC_DIR)/% $(HELPER_OBJS) | $(TMC_BUILD_DIR)
	@echo "Building library $@"
	@for file in $</*.c; do \
		$(CC) $(CFLAGS) -c $$file -o $(TMC_BUILD_DIR)/$$(basename $$file .c).o; \
	done
	$(AR) $(ARFLAGS) $@ $(TMC_BUILD_DIR)/*.o $(HELPER_OBJS)

# Clean build artifacts
clean:
	rm -rf $(TMC_BUILD_DIR)

.PHONY: all clean