VSCODE_SUPPORT = y

CC = gcc
SIZE = size
CFLAGS += -g3
CFLAGS += -DFTD2XX_PLATFORM

BINARY = $(BUILD_DIR)/$(PROJECT_NAME).elf

PROJECT_BUILD = $(BUILD_DIR)
export PROJECT_DIR
PROJECT_DIR = $(PROJECT)

TARGET = mpsse

EXTRA_LIBS += libmpsse.a

PLATFORM_DRIVERS := $(NO-OS)/drivers/platform/$(PLATFORM)/$(TARGET)

$(PLATFORM)_project:
	$(call mk_dir, $(BUILD_DIR)) $(HIDE)
	$(call copy_dir, $(PLATFORM_TOOLS)/.vscode, $(PROJECT)/.vscode) $(HIDE)
	python $(PLATFORM_TOOLS)/config_build.py $(NO-OS) $(PROJECT) $(BINARY) $(HIDE)

$(PLATFORM)_run:
	$(BINARY)

$(PLATFORM)_sdkopen:
	code $(PROJECT)

$(PLATFORM)_reset:
	$(call remove_dir,$(PROJECT)/.vscode)

$(PLATFORM)_post_build:
