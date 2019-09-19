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
	char *attr_name;
	ssize_t (*exec)(char *buf, size_t len, const struct channel_info *channel);
	struct attribute_map *map_in; 	/* in */
	struct attribute_map *map_out; /* out */
}attribute_map;


#endif /* SRC_DRIVERS_TINYIIOD_TINYIIOD_TYPES_H_ */
