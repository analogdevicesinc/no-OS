LVGL_PATH = $(NO-OS)/libraries/lvgl/lvgl

CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

include $(LVGL_PATH)/lvgl.mk
include $(LVGL_PATH)/component.mk
