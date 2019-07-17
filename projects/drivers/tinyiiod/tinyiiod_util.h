#ifndef __TINYIIOD_H__
#define __TINYIIOD_H__

#include <stdio.h>
#include <stdbool.h>

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

struct channel_info {
	int32_t ch_num;
	bool ch_out;
};

typedef struct attrtibute_map {
	char *attr_name;
	ssize_t (*exec)(char *buf, size_t len, const struct channel_info *channel);
	struct attrtibute_map *map; 	/* in */
	struct attrtibute_map *map_out; /* out */
}attrtibute_map;

bool strequal(const char *str1, const char *str2);
int32_t read_value(const char *str);
uint32_t read_ul_value(const char *str);
ssize_t rd_wr_attribute(element_info *el_info, char *buf, size_t len, attrtibute_map *map, bool is_write);
#endif /* __TINYIIOD_H__ */
