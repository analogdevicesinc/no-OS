ifeq (y,$(strip $(SWIOT1L_MQTT)))
CFLAGS += -DSWIOT1L_MQTT
SRC_DIRS += $(NO-OS)/libraries/mqtt
SRCS += $(PROJECT)/src/examples/swiot1l-mqtt/swiot1l_mqtt.c
INCS += $(PROJECT)/src/examples/swiot1l-mqtt/swiot1l_mqtt.h
endif

ifeq (y,$(strip $(SWIOT1L_FIRMWARE)))
CFLAGS += -DSWIOT1L_FIRMWARE
endif
