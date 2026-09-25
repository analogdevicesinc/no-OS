/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_coprocessor.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the coprocessor driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the coprocessor
 *        functions. If this is needed, it is suggested that this file and all of the other
 *        driver wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_coprocessor.h>

int capi_coprocessor_init(struct capi_coprocessor_handle **handle,
			  const struct capi_coprocessor_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init) {
		return -EINVAL;
	}
	return config->ops->init(handle, config);
}

int capi_coprocessor_deinit(struct capi_coprocessor_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit) {
		return -EINVAL;
	}
	return handle->ops->deinit(handle);
}

int capi_coprocessor_boot(struct capi_coprocessor_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->boot) {
		return -EINVAL;
	}
	return handle->ops->boot(handle);
}

int capi_coprocessor_halt(struct capi_coprocessor_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->halt) {
		return -EINVAL;
	}
	return handle->ops->halt(handle);
}

int capi_coprocessor_get_boot_caps(struct capi_coprocessor_handle *handle,
				   uint32_t *caps)
{
	if (!handle || !handle->ops || !caps) {
		return -EINVAL;
	}
	if (!handle->ops->get_boot_caps) {
		return -ENOTSUP;
	}
	return handle->ops->get_boot_caps(handle, caps);
}

int capi_coprocessor_set_boot_config(struct capi_coprocessor_handle *handle,
				     const struct capi_coprocessor_boot_config *config)
{
	if (!handle || !handle->ops || !config) {
		return -EINVAL;
	}
	if (!handle->ops->set_boot_config) {
		return -ENOTSUP;
	}
	return handle->ops->set_boot_config(handle, config);
}

int capi_coprocessor_get_boot_config(struct capi_coprocessor_handle *handle,
				     struct capi_coprocessor_boot_config *config)
{
	if (!handle || !handle->ops || !config) {
		return -EINVAL;
	}
	if (!handle->ops->get_boot_config) {
		return -ENOTSUP;
	}
	return handle->ops->get_boot_config(handle, config);
}

/**
 * @brief Update a single boot configuration field.
 *
 * Reads the current configuration, applies the caller's value to one field and
 * writes it back, so a driver only has to provide get_boot_config() and
 * set_boot_config() to get the granular setters as well.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] cap Capability bit for the field being written.
 * @param[in] value Value to store.
 *
 * @return 0 or negative errno.
 */
static int capi_coprocessor_set_boot_field(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_boot_cap cap, uint64_t value)
{
	struct capi_coprocessor_boot_config config;
	uint32_t caps;
	int ret;

	ret = capi_coprocessor_get_boot_caps(handle, &caps);
	if (ret) {
		return ret;
	}
	if (!(caps & (uint32_t)cap)) {
		return -ENOTSUP;
	}

	ret = capi_coprocessor_get_boot_config(handle, &config);
	if (ret) {
		return ret;
	}

	switch (cap) {
	case CAPI_COPROCESSOR_BOOT_CAP_VECTOR_BASE:
		config.vector_base = value;
		break;
	case CAPI_COPROCESSOR_BOOT_CAP_ENTRY_PC:
		config.entry_pc = value;
		break;
	default:
		return -EINVAL;
	}

	return capi_coprocessor_set_boot_config(handle, &config);
}

/**
 * @brief Read a single boot configuration field.
 *
 * @param[in] handle Coprocessor handle.
 * @param[in] cap Capability bit for the field being read.
 * @param[out] value Receives the field value.
 *
 * @return 0 or negative errno.
 */
static int capi_coprocessor_get_boot_field(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_boot_cap cap, uint64_t *value)
{
	struct capi_coprocessor_boot_config config;
	uint32_t caps;
	int ret;

	if (!value) {
		return -EINVAL;
	}

	ret = capi_coprocessor_get_boot_caps(handle, &caps);
	if (ret) {
		return ret;
	}
	if (!(caps & (uint32_t)cap)) {
		return -ENOTSUP;
	}

	ret = capi_coprocessor_get_boot_config(handle, &config);
	if (ret) {
		return ret;
	}

	switch (cap) {
	case CAPI_COPROCESSOR_BOOT_CAP_VECTOR_BASE:
		*value = config.vector_base;
		break;
	case CAPI_COPROCESSOR_BOOT_CAP_ENTRY_PC:
		*value = config.entry_pc;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

int capi_coprocessor_set_vector_base(struct capi_coprocessor_handle *handle,
				     uint64_t vector_base)
{
	return capi_coprocessor_set_boot_field(handle,
					       CAPI_COPROCESSOR_BOOT_CAP_VECTOR_BASE,
					       vector_base);
}

int capi_coprocessor_get_vector_base(struct capi_coprocessor_handle *handle,
				     uint64_t *vector_base)
{
	return capi_coprocessor_get_boot_field(handle,
					       CAPI_COPROCESSOR_BOOT_CAP_VECTOR_BASE,
					       vector_base);
}

int capi_coprocessor_set_entry_pc(struct capi_coprocessor_handle *handle,
				  uint64_t entry_pc)
{
	return capi_coprocessor_set_boot_field(handle,
					       CAPI_COPROCESSOR_BOOT_CAP_ENTRY_PC,
					       entry_pc);
}

int capi_coprocessor_get_entry_pc(struct capi_coprocessor_handle *handle,
				  uint64_t *entry_pc)
{
	return capi_coprocessor_get_boot_field(handle,
					       CAPI_COPROCESSOR_BOOT_CAP_ENTRY_PC,
					       entry_pc);
}

int capi_coprocessor_set_mode(struct capi_coprocessor_handle *handle,
			      enum capi_coprocessor_mode mode)
{
	if (!handle || !handle->ops) {
		return -EINVAL;
	}
	if (!handle->ops->set_mode) {
		return -ENOTSUP;
	}
	return handle->ops->set_mode(handle, mode);
}

int capi_coprocessor_get_mode(struct capi_coprocessor_handle *handle,
			      enum capi_coprocessor_mode *mode)
{
	if (!handle || !handle->ops || !mode) {
		return -EINVAL;
	}
	if (!handle->ops->get_mode) {
		return -ENOTSUP;
	}
	return handle->ops->get_mode(handle, mode);
}

int capi_coprocessor_resource_get_count(struct capi_coprocessor_handle *handle,
					enum capi_coprocessor_resource_type type, uint32_t *count)
{
	if (!handle || !handle->ops || !count) {
		return -EINVAL;
	}
	if (!handle->ops->resource_get_count) {
		return -ENOTSUP;
	}
	return handle->ops->resource_get_count(handle, type, count);
}

int capi_coprocessor_resource_set_enabled(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_resource_type type, uint32_t index,
		bool enable)
{
	if (!handle || !handle->ops) {
		return -EINVAL;
	}
	if (!handle->ops->resource_set_enabled) {
		return -ENOTSUP;
	}
	return handle->ops->resource_set_enabled(handle, type, index, enable);
}

int capi_coprocessor_resource_get_enabled(struct capi_coprocessor_handle
		*handle,
		enum capi_coprocessor_resource_type type, uint32_t index,
		bool *enabled)
{
	if (!handle || !handle->ops || !enabled) {
		return -EINVAL;
	}
	if (!handle->ops->resource_get_enabled) {
		return -ENOTSUP;
	}
	return handle->ops->resource_get_enabled(handle, type, index, enabled);
}
