LVGL_PATH = $(NO-OS)/libraries/lvgl/lvgl
CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

include $(LVGL_PATH)/lvgl.mk
include $(LVGL_PATH)/component.mk

LVGL_HEADER_PATH = $(LVGL_PATH)/src
CFLAGS += -DLV_CONF_PATH=$(PROJECT)/lv_conf.h
SRCS += $(wildcard $(LVGL_HEADER_PATH)/*.c)

COMPONENT_SRCDIRS := $(addprefix $(LVGL_PATH)/, $(COMPONENT_SRCDIRS))
INCS += $(foreach dir,$(COMPONENT_SRCDIRS),$(wildcard $(dir)/*.h))
SRCS += $(foreach dir,$(COMPONENT_SRCDIRS),$(wildcard $(dir)/*.c))
