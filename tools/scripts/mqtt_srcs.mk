CFLAGS += -DMQTTCLIENT_PLATFORM_HEADER="mqtt_noos_support.h"

MQTT_DIR	= $(NO-OS)/libraries/mqtt
PAHO_DIR	= $(MQTT_DIR)/paho.mqtt.embedded-c

SRC_DIRS += $(PAHO_DIR)/MQTTPacket/src

SRCS += $(MQTT_DIR)/mqtt_client.c \
	$(MQTT_DIR)/mqtt_noos_support.c \
	$(PAHO_DIR)/MQTTClient-C/src/MQTTClient.c

INCS += $(MQTT_DIR)/mqtt_client.h \
	$(MQTT_DIR)/mqtt_noos_support.h \
	$(PAHO_DIR)/MQTTClient-C/src/MQTTClient.h