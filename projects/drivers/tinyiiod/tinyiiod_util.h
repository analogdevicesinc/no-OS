#ifndef __TINYIIOD_H__
#define __TINYIIOD_H__

#include <stdio.h>
#include <stdbool.h>

struct channel_info {
	int32_t ch_num;
	bool ch_out;
};

struct attrtibute_map {
	char *attr_name;
	ssize_t (*exec)(char *buf, size_t len, const struct channel_info *channel);
};

ssize_t ch_exec_read_attr(const char *channel,
			    bool ch_out, const char *attr, char *buf, size_t len, struct attrtibute_map *map);
ssize_t ch_exec_write_attr(const char *channel,
			    bool ch_out, const char *attr, const char *buf, size_t len, struct attrtibute_map *map);
bool strequal(const char *str1, const char *str2);
int32_t read_value(const char *str);
uint32_t read_ul_value(const char *str);
int16_t get_attribute_id(const char *attr, const struct attrtibute_map* map);
ssize_t read_all_attr(char *buf, size_t len,
			     const struct channel_info *channel, const struct attrtibute_map* map);
ssize_t write_all_attr(char *buf, size_t len,
			      const struct channel_info *channel, const struct attrtibute_map* map);

#endif /* __TINYIIOD_H__ */
