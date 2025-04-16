LVGL_PATH = $(NO-OS)/libraries/lvgl/lvgl
CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

include $(LVGL_PATH)/lvgl.mk
include $(LVGL_PATH)/component.mk

LVGL_HEADER_PATH = $(LVGL_PATH)/src

SRCS += $(wildcard $(LVGL_HEADER_PATH)/*.c)

# Check if LVGL config file is present
ifeq ($(wildcard $(LVGL_CONFIG_PATH)),)
$(error File lv_conf.h does not exists)
else
$(info Found lv_conf.h config file)
CFLAGS += -DLV_CONF_PATH=\"$(LVGL_CONFIG_PATH)\"
endif

COMPONENT_SRCDIRS := $(addprefix $(LVGL_PATH)/, $(COMPONENT_SRCDIRS))
INCS += $(foreach dir,$(COMPONENT_SRCDIRS),$(wildcard $(dir)/*.h))
SRCS += $(foreach dir,$(COMPONENT_SRCDIRS),$(wildcard $(dir)/*.c))
