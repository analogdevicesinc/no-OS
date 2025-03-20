# Define version variables
LVGL_VERSION_MAJOR := 9
LVGL_VERSION_MINOR := 3
LVGL_VERSION_PATCH := 0
LVGL_VERSION_INFO  := dev

# Combine version variables into a single version string
LVGL_VERSION := $(LVGL_VERSION_MAJOR).$(LVGL_VERSION_MINOR).$(LVGL_VERSION_PATCH)

# Export the version as an environment variable
export LVGL_VERSION := $(LVGL_VERSION)

# Define the SOVERSION
LVGL_SOVERSION := $(LVGL_VERSION_MAJOR)

CFLAGS += -I$(NO-OS)/libraries/lvgl/lvgl
CFLAGS += -I$(NO-OS)/libraries/lvgl/lvgl/src
CFLAGS += -I$(NO-OS)/libraries/lvgl  # Add this line to include the directory containing lv_conf.h

# Source files
SOURCES := $(wildcard $(NO-OS)/libraries/lvgl/lvgl/src/*.c)

# Add sources to the build
OBJS := $(SOURCES:.c=.o)

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

CFLAGS += -DLV_KCONFIG_IGNORE
CFLAGS += -DLV_CONF_SKIP