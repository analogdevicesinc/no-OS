/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_fdsp.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the FastDSP driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the FastDSP functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_fdsp.h>

int capi_fdsp_init(struct capi_fdsp_handle **handle, const struct capi_fdsp_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_fdsp_deinit(const struct capi_fdsp_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_fdsp_start(const struct capi_fdsp_handle *handle, bool *exec_state)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->start != NULL) {
		return handle->ops->start(handle, exec_state);
	}

	return -EINVAL;
}

int capi_fdsp_stop(const struct capi_fdsp_handle *handle, bool *exec_state)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->stop != NULL) {
		return handle->ops->stop(handle, exec_state);
	}

	return -EINVAL;
}

int capi_fdsp_resume(const struct capi_fdsp_handle *handle, bool *exec_state)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->resume != NULL) {
		return handle->ops->resume(handle, exec_state);
	}

	return -EINVAL;
}

int capi_fdsp_set_parity_check(const struct capi_fdsp_handle *handle, bool check)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_parity_check != NULL) {
		return handle->ops->set_parity_check(handle, check);
	}

	return -EINVAL;
}

int capi_fdsp_switch_bank_immediate(const struct capi_fdsp_handle *handle,
				    enum capi_fdsp_bank_sel bank, bool zero)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->switch_bank_immediate != NULL) {
		return handle->ops->switch_bank_immediate(handle, bank, zero);
	}

	return -EINVAL;
}

int capi_fdsp_switch_bank_ramp(const struct capi_fdsp_handle *handle, enum capi_fdsp_bank_sel bank,
			       bool zero, float stop, enum capi_fdsp_ramp_rate rate)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->switch_bank_ramp != NULL) {
		return handle->ops->switch_bank_ramp(handle, bank, zero, stop, rate);
	}

	return -EINVAL;
}

int capi_fdsp_copy_bank(const struct capi_fdsp_handle *handle, enum capi_fdsp_bank_sel source,
			enum capi_fdsp_bank_sel destination)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->copy_bank != NULL) {
		return handle->ops->copy_bank(handle, source, destination);
	}

	return -EINVAL;
}

int capi_fdsp_set_envelope(const struct capi_fdsp_handle *handle,
			   enum capi_fdsp_operation operation, float attack, float decay)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_envelope != NULL) {
		return handle->ops->set_envelope(handle, operation, attack, decay);
	}

	return -EINVAL;
}

int capi_fdsp_set_modulus_condition(const struct capi_fdsp_handle *handle, uint8_t modulus)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_modulus_condition != NULL) {
		return handle->ops->set_modulus_condition(handle, modulus);
	}

	return -EINVAL;
}

int capi_fdsp_set_condition(const struct capi_fdsp_handle *handle,
			    enum capi_fdsp_conditional_execution condition)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_condition != NULL) {
		return handle->ops->set_condition(handle, condition);
	}

	return -EINVAL;
}

int capi_fdsp_clear_condition(const struct capi_fdsp_handle *handle,
			      enum capi_fdsp_conditional_execution condition)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->clear_condition != NULL) {
		return handle->ops->clear_condition(handle, condition);
	}

	return -EINVAL;
}

int capi_fdsp_safeload_params(const struct capi_fdsp_handle *handle, uint32_t address,
			      const struct capi_fdsp_params *parameters)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->safeload_params != NULL) {
		return handle->ops->safeload_params(handle, address, parameters);
	}

	return -EINVAL;
}