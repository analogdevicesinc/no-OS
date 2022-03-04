CC ?= gcc

BINARY		= $(BUILD_DIR)/$(PROJECT_NAME).out

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1
PROJECT_BUILD = $(BUILD_DIR)/app

CFLAGS +=  -g3 \
		-DLINUX_PLATFORM \

$(PROJECT_TARGET):
	$(MUTE) $(call mk_dir, $(BUILD_DIR)) $(HIDE)
	$(MUTE) $(call set_one_time_rule,$@)

$(PLATFORM)_sdkopen:
	$(MUTE) $(call mk_dir, $(PROJECT_BUILD)) $(HIDE)
	$(MUTE) $(call copy_dir, $(PLATFORM_TOOLS)/.vscode, $(PROJECT_BUILD)/.vscode) $(HIDE)
	$(MUTE) python $(PLATFORM_TOOLS)/config_build.py $(NO-OS) $(PROJECT) $(BINARY) $(HIDE)
	$(call print, Workspace ready. Open $(PROJECT_BUILD) directory in VSCode for debug)

linux_run: $(BINARY)
	$(BINARY)
