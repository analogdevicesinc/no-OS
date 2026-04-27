/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_owm.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the OWM (One-Wire Master) driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the OWM functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_owm.h>

int capi_owm_init(struct capi_owm_handle **handle, const struct capi_owm_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init) {
		return -EINVAL;
	}
	return config->ops->init(handle, config);
}

int capi_owm_deinit(struct capi_owm_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit) {
		return -EINVAL;
	}
	return handle->ops->deinit(handle);
}

int capi_owm_set_speed(struct capi_owm_handle *handle, enum capi_owm_speed speed)
{
	if (!handle || !handle->ops || !handle->ops->set_speed) {
		return -EINVAL;
	}
	return handle->ops->set_speed(handle, speed);
}

int capi_owm_get_speed(struct capi_owm_handle *handle, enum capi_owm_speed *speed)
{
	if (!handle || !handle->ops || !handle->ops->get_speed) {
		return -EINVAL;
	}
	return handle->ops->get_speed(handle, speed);
}

int capi_owm_set_strong_pullup(struct capi_owm_handle *handle, bool enable)
{
	if (!handle || !handle->ops || !handle->ops->set_strong_pullup) {
		return -EINVAL;
	}
	return handle->ops->set_strong_pullup(handle, enable);
}

int capi_owm_reset_bus(struct capi_owm_handle *handle, bool *presence)
{
	if (!handle || !handle->ops || !handle->ops->reset_bus) {
		return -EINVAL;
	}
	return handle->ops->reset_bus(handle, presence);
}

int capi_owm_search(struct capi_owm_handle *handle, bool new_search, uint8_t family_code,
		    uint8_t *rom_id, bool *found)
{
	if (!handle || !handle->ops || !handle->ops->search) {
		return -EINVAL;
	}
	return handle->ops->search(handle, new_search, family_code, rom_id, found);
}

int capi_owm_search_alarm(struct capi_owm_handle *handle, bool new_search, uint8_t family_code,
			  uint8_t *rom_id, bool *found)
{
	if (!handle || !handle->ops || !handle->ops->search_alarm) {
		return -EINVAL;
	}
	return handle->ops->search_alarm(handle, new_search, family_code, rom_id, found);
}

int capi_owm_read_rom(struct capi_owm_handle *handle, uint8_t *rom_id)
{
	if (!handle || !handle->ops || !handle->ops->read_rom) {
		return -EINVAL;
	}
	return handle->ops->read_rom(handle, rom_id);
}

int capi_owm_match_rom(struct capi_owm_handle *handle, const uint8_t *rom_id)
{
	if (!handle || !handle->ops || !handle->ops->match_rom) {
		return -EINVAL;
	}
	return handle->ops->match_rom(handle, rom_id);
}

int capi_owm_skip_rom(struct capi_owm_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->skip_rom) {
		return -EINVAL;
	}
	return handle->ops->skip_rom(handle);
}

int capi_owm_resume(struct capi_owm_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->resume) {
		return -EINVAL;
	}
	return handle->ops->resume(handle);
}

int capi_owm_overdrive_match_rom(struct capi_owm_handle *handle, const uint8_t *rom_id)
{
	if (!handle || !handle->ops || !handle->ops->overdrive_match_rom) {
		return -EINVAL;
	}
	return handle->ops->overdrive_match_rom(handle, rom_id);
}

int capi_owm_overdrive_skip_rom(struct capi_owm_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->overdrive_skip_rom) {
		return -EINVAL;
	}
	return handle->ops->overdrive_skip_rom(handle);
}

int capi_owm_write_bit(struct capi_owm_handle *handle, uint8_t bit)
{
	if (!handle || !handle->ops || !handle->ops->write_bit) {
		return -EINVAL;
	}
	return handle->ops->write_bit(handle, bit);
}

int capi_owm_read_bit(struct capi_owm_handle *handle, uint8_t *bit)
{
	if (!handle || !handle->ops || !handle->ops->read_bit) {
		return -EINVAL;
	}
	return handle->ops->read_bit(handle, bit);
}

int capi_owm_write_byte(struct capi_owm_handle *handle, uint8_t byte)
{
	if (!handle || !handle->ops || !handle->ops->write_byte) {
		return -EINVAL;
	}
	return handle->ops->write_byte(handle, byte);
}

int capi_owm_read_byte(struct capi_owm_handle *handle, uint8_t *byte)
{
	if (!handle || !handle->ops || !handle->ops->read_byte) {
		return -EINVAL;
	}
	return handle->ops->read_byte(handle, byte);
}

int capi_owm_write_block(struct capi_owm_handle *handle, const uint8_t *buf, uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->write_block) {
		return -EINVAL;
	}
	return handle->ops->write_block(handle, buf, len);
}

int capi_owm_read_block(struct capi_owm_handle *handle, uint8_t *buf, uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->read_block) {
		return -EINVAL;
	}
	return handle->ops->read_block(handle, buf, len);
}

int capi_owm_write_block_async(struct capi_owm_handle *handle, const uint8_t *buf, uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->write_block_async) {
		return -EINVAL;
	}
	return handle->ops->write_block_async(handle, buf, len);
}

int capi_owm_read_block_async(struct capi_owm_handle *handle, uint8_t *buf, uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->read_block_async) {
		return -EINVAL;
	}
	return handle->ops->read_block_async(handle, buf, len);
}

int capi_owm_get_device_count(struct capi_owm_handle *handle, uint32_t *count)
{
	if (!handle || !handle->ops || !handle->ops->get_device_count) {
		return -EINVAL;
	}
	return handle->ops->get_device_count(handle, count);
}

int capi_owm_crc8(struct capi_owm_handle *handle, const uint8_t *data, uint32_t len, uint8_t *crc)
{
	if (!handle || !handle->ops || !handle->ops->crc8) {
		return -EINVAL;
	}
	return handle->ops->crc8(handle, data, len, crc);
}

int capi_owm_register_callback(struct capi_owm_handle *handle, capi_owm_callback const callback,
			       void *const callback_arg)
{
	if (!handle || !handle->ops || !handle->ops->register_callback) {
		return -EINVAL;
	}
	return handle->ops->register_callback(handle, callback, callback_arg);
}

void capi_owm_isr(void *handle)
{
	struct capi_owm_handle *owm_handle = (struct capi_owm_handle *)handle;

	if (!handle || !owm_handle->ops || !owm_handle->ops->isr) {
		return;
	}
	owm_handle->ops->isr(handle);
}
