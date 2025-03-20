include $(NO-OS)/libraries/lvgl/lvgl/component.mk
include $(NO-OS)/libraries/lvgl/lvgl/lvgl.mk

# Append a prefix path to every item in COMPONENT_SRCDIRS
COMPONENT_SRCDIRS := $(addprefix $(NO-OS)/libraries/lvgl/lvgl/,$(COMPONENT_SRCDIRS))
# Filter out directories that don't exist
COMPONENT_SRCDIRS := $(filter $(wildcard $(COMPONENT_SRCDIRS)), $(COMPONENT_SRCDIRS))
# Append a prefix path to every item in COMPONENT_ADD_INCLUDEDIRS
COMPONENT_ADD_INCLUDEDIRS := $(addprefix $(NO-OS)/libraries/lvgl/lvgl/,$(COMPONENT_ADD_INCLUDEDIRS))
# Filter out directories that don't exist from COMPONENT_ADD_INCLUDEDIRS
COMPONENT_ADD_INCLUDEDIRS := $(filter $(wildcard $(COMPONENT_ADD_INCLUDEDIRS)), $(COMPONENT_ADD_INCLUDEDIRS))

# Find and store all .c files from COMPONENT_SRCDIRS directories
SRCS_LVGL = $(foreach dir,$(COMPONENT_SRCDIRS),$(wildcard $(dir)/*.c))

# Find and store all .h files from COMPONENT_ADD_INCLUDEDIRS directories
HDRS_LVGL = $(foreach dir,$(COMPONENT_ADD_INCLUDEDIRS),$(wildcard $(dir)/*.h))

INCS += $(HDRS_LVGL)
SRCS += $(SRCS_LVGL)

CFLAGS += -DLV_KCONFIG_IGNORE
CFLAGS += -DLV_CONF_SKIP