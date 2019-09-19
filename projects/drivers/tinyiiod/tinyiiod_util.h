#ifndef __TINYIIOD_H__
#define __TINYIIOD_H__

#include <stdbool.h>
#include "tinyiiod_adc.h"
#include "tinyiiod_dac.h"
#include "tinyiiod_types.h"

typedef struct tinyiiod_device {
	char *name;
	void *pointer;
	uint16_t number_of_channels;
	ssize_t (*get_device_xml)(char** xml,  char *device_name, uint8_t ch_no);
}tinyiiod_device;

typedef struct tinyiiod_devices {
	tinyiiod_device **devices;
	uint8_t number_of_dev;
}tinyiiod_devices;


enum elem_level {
	DEVICE_EL,
	CHANNEL_EL,
	ATTRIBUTE_EL,
	MAX_EL
};

typedef struct element_info {
	const char *name[MAX_EL];
	bool ch_out;
	enum elem_level crnt_level;
}element_info;

ssize_t rd_wr_attribute(element_info *el_info, char *buf, size_t len, attribute_map *map, bool is_write);
ssize_t tinyiiod_register_device(void* device_address, const char *device_name, uint16_t number_of_channels, ssize_t (*get_device_xml)(char** xml,  char *device_name, uint8_t ch_no));
ssize_t get_xml(char **outxml);

#endif /* __TINYIIOD_H__ */
