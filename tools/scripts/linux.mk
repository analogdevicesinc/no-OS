CC = gcc

BINARY		= $(BUILD_DIR)/$(PROJECT_NAME).out

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1

CFLAGS +=  -g3 \
		-DLINUX_PLATFORM \

$(PROJECT_TARGET):
	$(MUTE) $(call mk_dir, $(BUILD_DIR)) $(HIDE)
	$(MUTE) $(call set_one_time_rule,$@)

linux_run: $(BINARY)
	$(BINARY)
