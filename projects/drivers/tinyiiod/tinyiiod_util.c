#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <xil_io.h>
#include "tinyiiod.h"
#include "tinyiiod_util.h"

/**
 * Get channel number
 * @param *ch channel name + channel number
 * @return channel number Ex: for "altvoltage0" return 0, for "voltage2" return 2
 */
static int32_t get_channel_number(const char *ch)
{
	char *p = (char*)ch;
	int32_t ch_num = 0;

	while(*p) {
		if(isdigit(*p))
			ch_num = strtol(p, &p, 10);
		else
			p++;
	}

	return ch_num;
}

/**
 * Get attribute ID from map based on attribute name
 * @param attr* attribute name
 * @param *map pointer to map terminated with NULL element
 * @param map_size map_size element numbers of the map
 * @return attribute ID, or negative value if attribute is not found
 */
static int16_t get_attribute_id(const char *attr, const attrtibute_map* map)
{
	int16_t i = 0;

	if(!map)
		return -EINVAL;

	while(map[i].attr_name)
	{
		if (strequal(attr, map[i].attr_name))
			return i;
		i++;
	}

	return -ENODEV;
}

/**
 * read all attributes from map
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @param *map map with coresponding attributes
 * @param map_size
 * @return length of chars written in buf
 */
static ssize_t read_all_attr(char *buf, size_t len,
			     const struct channel_info *channel, const attrtibute_map* map)
{
	int16_t i = 0, j = 0;
	char local_buf[0x1000];
	while(map[i].attr_name)
	{
		int16_t attr_length = map[i].exec((local_buf), len, channel);
		int32_t *len = (int32_t *)(buf + j);
		*len = Xil_EndianSwap32(attr_length);

		j += 4;
		if(attr_length >= 0) {
			sprintf(buf + j, "%s", local_buf);
			if (attr_length & 0x3) /* multiple of 4 */
				attr_length = ((attr_length >> 2) + 1) << 2;
			j += attr_length;
		}
		i++;
	}

	return j;
}

/**
 * write all attributes from map
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @param *map map with coresponding attributes
 * @param map_size
 * @return length of chars written in buf
 */
static ssize_t write_all_attr(char *buf, size_t len,
			      const struct channel_info *channel, const attrtibute_map* map)
{
	int16_t i = 0, j = 0;

	while(map[i].attr_name)
	{
		int16_t attr_length = Xil_EndianSwap32((uint32_t)(buf + j));
		j += 4;
		map[i].exec((buf + j), attr_length, channel);
		j += attr_length;
		if (j & 0x3)
			j = ((j >> 2) + 1) << 2;
		i++;
	}

	return len;
}

/**
 * read write attribute
 * @param *device name
 * @param *channel name
 * @param *ch_out type: input/output
 * @param *attr name
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @return length of chars written in buf
 */
ssize_t rd_wr_attribute(element_info *el_info, char *buf, size_t len, attrtibute_map *map, bool is_write)
{
	int16_t attribute_id;

	if(!map)
		return -ENOENT;

	if(el_info->crnt_level == CHANNEL_EL && strequal(el_info->name[CHANNEL_EL], ""))
		el_info->crnt_level++;

	attribute_id = get_attribute_id(el_info->name[el_info->crnt_level], map);

	if(attribute_id < 0 && el_info->crnt_level == ATTRIBUTE_EL && strequal(el_info->name[el_info->crnt_level], "")) /* element not fond, but try to rd/wr all if element is ATTRIBUTE_EL and name is "" */
	{
		const struct channel_info channel_info = {
			get_channel_number(el_info->name[CHANNEL_EL]),
			el_info->ch_out
		};
		if(is_write)
			return write_all_attr(buf, len, &channel_info, map);
		else
			return read_all_attr(buf, len, &channel_info, map);
	}
	if(attribute_id >= 0)  /* element fond */
	{
		if(map[attribute_id].exec)
		{
			const struct channel_info channel_info = {
					get_channel_number(el_info->name[CHANNEL_EL]),
					el_info->ch_out
				};
			return map[attribute_id].exec((char*)buf, len, &channel_info);
		}
		else
		{
			el_info->crnt_level++;
			return rd_wr_attribute(el_info, buf, len, el_info->ch_out ? map[attribute_id].map_out : map[attribute_id].map_in, is_write);
		}
	}

	return -ENOENT;
}

/**
 * Compare two strings
 * @param *str1 pointer to string 1
 * @param *str2 pointer to string 2
 * @return TRUE if strings are equal, 0 otherwise
 */
bool strequal(const char *str1, const char *str2)
{
	return !strcmp(str1, str2);
}

/**
 * Converts from string to long value
 * @param *str
 * @return long value
 */
int32_t read_value(const char *str)
{
	char *end;
	int32_t value = strtol(str, &end, 0);

	if (end == str)
		return -EINVAL;
	else
		return value;
}

/**
 * Converts from string to unsigned long value
 * @param *str
 * @return long value
 */
uint32_t read_ul_value(const char *str)
{
	char *end;
	uint32_t value = strtoul(str, &end, 0);

	if (end == str)
		return -EINVAL;
	else
		return value;
}
