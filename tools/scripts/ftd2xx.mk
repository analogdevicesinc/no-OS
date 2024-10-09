
CC=gcc
SIZE = size
CFLAGS += -g3

PROJECT_BUILD = $(BUILD_DIR)

TARGET = mpsse

EXTRA_LIBS += libmpsse.a

PLATFORM_DRIVERS := $(NO-OS)/drivers/platform/$(PLATFORM)/$(TARGET)

$(PLATFORM)_project:
	$(call mk_dir, $(BUILD_DIR)) $(HIDE)

$(PLATFORM)_sdkopen:
	$(call mk_dir, $(PROJECT_BUILD)) $(HIDE)
	$(call copy_dir, $(PLATFORM_TOOLS)/.vscode, $(PROJECT_BUILD)/.vscode) $(HIDE)
	python $(PLATFORM_TOOLS)/config_build.py $(NO-OS) $(PROJECT) $(BINARY) $(HIDE)
	$(call print, Workspace ready. Open $(PROJECT_BUILD) directory in VSCode for debug)

$(PLATFORM)_reset:

$(PLATFORM)_post_build:
