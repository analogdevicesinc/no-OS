/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAX20303_CAPI_H_
#define MAX20303_CAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "capi_i2c.h"

#define MAX20303_I2C_ADDR	0x28
#define MAX20303_FG_I2C_ADDR	0x36

#define MAX20303_VCELL_REG	0x02
#define MAX20303_SOC_REG	0x04
#define MAX20303_MODE_REG	0x06
#define MAX20303_VERSION_REG	0x08
#define MAX20303_HIBRT_REG	0x0A
#define MAX20303_CONFIG_REG	0x0C
#define MAX20303_VALRT_REG	0x14
#define MAX20303_CRATE_REG	0x16
#define MAX20303_VRESET_REG	0x18
#define MAX20303_STATUS_REG	0x1A
#define MAX20303_TABLE_REG(x)	(0x40 + (x))
#define MAX20303_CMD_REG	0xFE

#define MAX20303_VCELL_LSB_nV	78125

#define MAX20303_HIBERNATE_EN	0xFFFF
#define MAX20303_HIBERNATE_DIS	0x0

/**
 * MAX20303 CAPI initialization parameters
 */
struct max20303_capi_init_param {
	/** I2C controller handle */
	struct capi_i2c_controller_handle *i2c_controller;
	/** I2C device for main interface */
	struct capi_i2c_device *i2c_device;
	/** I2C device for fuel gauge interface */
	struct capi_i2c_device *fg_i2c_device;
};

/**
 * MAX20303 CAPI device descriptor
 */
struct max20303_capi_desc {
	/** I2C controller handle */
	struct capi_i2c_controller_handle *i2c_controller;
	/** I2C device for main interface */
	struct capi_i2c_device i2c_device;
	/** I2C device for fuel gauge interface */
	struct capi_i2c_device fg_i2c_device;
};

/**
 * @brief Initialize the MAX20303 driver
 * @param desc - Pointer to device descriptor pointer
 * @param param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int max20303_init(struct max20303_capi_desc **desc,
		  struct max20303_capi_init_param *param);

/**
 * @brief Free resources allocated by max20303_init()
 * @param desc - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int max20303_remove(struct max20303_capi_desc *desc);

/**
 * @brief Write to MAX20303 register
 * @param desc - Device descriptor
 * @param addr - Register address
 * @param val - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int max20303_reg_write(struct max20303_capi_desc *desc, uint8_t addr, uint16_t val);

/**
 * @brief Read from MAX20303 register
 * @param desc - Device descriptor
 * @param addr - Register address
 * @param val - Pointer to store read value
 * @return 0 in case of success, negative error code otherwise
 */
int max20303_reg_read(struct max20303_capi_desc *desc, uint8_t addr, uint16_t *val);

/**
 * @brief Read battery cell voltage
 * @param desc - Device descriptor
 * @param vcell_uv - Pointer to store voltage in microvolts
 * @return 0 in case of success, negative error code otherwise
 */
int max20303_read_vcell(struct max20303_capi_desc *desc, uint32_t *vcell_uv);

/**
 * @brief Set hibernate mode
 * @param desc - Device descriptor
 * @param enable - true to enable, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max20303_set_hibernate(struct max20303_capi_desc *desc, bool enable);

/**
 * @brief Read battery state of charge
 * @param desc - Device descriptor
 * @param percentage - Pointer to store percentage (0-100)
 * @return 0 in case of success, negative error code otherwise
 */
int max20303_read_soc(struct max20303_capi_desc *desc, uint16_t *percentage);

#ifdef __cplusplus
}
#endif

#endif /* MAX20303_CAPI_H_ */
