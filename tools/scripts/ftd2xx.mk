
CC=gcc
SIZE = size
CFLAGS += -g3

PROJECT_BUILD = $(BUILD_DIR)

TARGET = mpsse

EXTRA_LIBS += libmpsse.a

PLATFORM_DRIVERS := $(NO-OS)/drivers/platform/$(PLATFORM)/$(TARGET)

$(PLATFORM)_project:
	$(call mk_dir, $(BUILD_DIR)) $(HIDE)

$(PLATFORM)_reset:

$(PLATFORM)_post_build:
