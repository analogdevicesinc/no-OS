/*
 * tinyiiod_types.h
 *
 *  Created on: 19 Sep 2019
 *      Author: cristian
 */

#ifndef SRC_DRIVERS_TINYIIOD_TINYIIOD_TYPES_H_
#define SRC_DRIVERS_TINYIIOD_TINYIIOD_TYPES_H_

struct channel_info {
	int32_t ch_num;
	bool ch_out;
};

typedef struct attribute_map {
	const char *name;
	ssize_t (*exec)(void *device, char *buf, size_t len, const struct channel_info *channel);
	struct attribute_map *map_in; 	/* in */
	struct attribute_map *map_out; /* out */
	struct attribute_map *map_in_global;
	struct attribute_map *map_out_global;
}attribute_map;

typedef struct tinyiiod_device {
	const char *name;
	uint16_t number_of_channels;
	void *pointer;
	attribute_map *attr_map;
	ssize_t (*get_device_xml)(char** xml, const char *device_name, uint8_t ch_no);
}tinyiiod_device;

typedef struct tinyiiod_devices {
	tinyiiod_device **devices;
	uint8_t number_of_dev;
}tinyiiod_devices;




#endif /* SRC_DRIVERS_TINYIIOD_TINYIIOD_TYPES_H_ */
