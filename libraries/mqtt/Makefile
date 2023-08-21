remove_fun = rm -rf $(1)

MQTT_DIR	= $(realpath .)
PAHO_DIR	= $(MQTT_DIR)/paho.mqtt.embedded-c
PAHO_PACKET_DIR = $(PAHO_DIR)/MQTTPacket/src
PAHO_CLIENT_DIR = $(PAHO_DIR)/MQTTClient-C/src

SRCS = mqtt_client.c mqtt_noos_support.c
SRCS += $(PAHO_PACKET_DIR)/MQTTConnectClient.c\
	$(PAHO_PACKET_DIR)/MQTTDeserializePublish.c\
	$(PAHO_PACKET_DIR)/MQTTFormat.c\
	$(PAHO_PACKET_DIR)/MQTTPacket.c\
	$(PAHO_PACKET_DIR)/MQTTSerializePublish.c\
	$(PAHO_PACKET_DIR)/MQTTSubscribeClient.c\
	$(PAHO_PACKET_DIR)/MQTTUnsubscribeClient.c

SRCS += $(PAHO_CLIENT_DIR)/MQTTClient.c

OBJS = $(SRCS:.c=.o)

CFLAGS += -DMQTTCLIENT_PLATFORM_HEADER="mqtt_noos_support.h" \
		-I"$(PAHO_CLIENT_DIR)" \
		-I"$(PAHO_PACKET_DIR)" \
		-I"$(MQTT_DIR)"

LIBRARY = libmqtt.a

.PHONEY = all clean

ifeq ($(wildcard $(PAHO_DIR)/README.md),)
all:
	git submodule update --init --remote -- $(PAHO_DIR)
	$(MAKE) $(LIBRARY)
else
all: $(LIBRARY)
endif

$(LIBRARY): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	-$(call remove_fun,$(OBJS) $(LIBRARY))
