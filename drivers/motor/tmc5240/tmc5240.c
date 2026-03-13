/***************************************************************************//**
 *   @file   tmc5240.c
 *   @brief  Implementation of TMC5240 driver.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include <stdlib.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "tmc/ic/TMC5240/TMC5240.h"
#include "tmc5240.h"
#include "no_os_print_log.h"

/* Maximum number of TMC5240 devices supported */
#define TMC5240_MAX_DEVICES 4

/* Device lookup table for multi-instance support */
static struct tmc5240_dev *tmc5240_devices[TMC5240_MAX_DEVICES] = {NULL};

/**
 * @brief Find a free device ID slot
 * @return Device ID (0-3) or -ENOMEM if no slots available
 */
static int tmc5240_get_free_id(void)
{
	uint8_t i;

	for (i = 0; i < TMC5240_MAX_DEVICES; i++) {
		if (tmc5240_devices[i] == NULL)
			return i;
	}

	return -ENOMEM;
}

/**
 * @brief Get device instance by ID
 * @param icID Device ID
 * @return Pointer to device or NULL if not found
 */
static struct tmc5240_dev *tmc5240_get_device(uint16_t icID)
{
	if (icID >= TMC5240_MAX_DEVICES)
		return NULL;

	return tmc5240_devices[icID];
}

/**
 * @brief Read/Write SPI function for TMC-API
 *
 * This function is used to read and write data to the TMC ICs via SPI.
 *
 * @param icID ID of the IC.
 * @param data Pointer to the data storage.
 * @param dataLength Length of data to transfer.
 *
 */
void tmc5240_readWriteSPI(uint16_t icID, uint8_t *data, size_t dataLength)
{
	struct tmc5240_dev *dev;
	int ret;

	dev = tmc5240_get_device(icID);
	if (!dev || !dev->spi_desc)
		return;

	ret = no_os_spi_write_and_read(dev->spi_desc, data, dataLength);
	if (ret) {
		/* TMC-API doesn't support error return, so we can only log this */
		pr_err("Failed to read/write SPI for TMC5240 IC: %d\r\n", ret);
	}
}

/**
 * @brief Read/Write UART function for TMC-API (stub for SPI-only implementation)
 *
 * This function is required by the TMC-API interface but not used in this
 * SPI-only implementation. It always returns false to indicate UART is not available.
 *
 * @param icID ID of the IC.
 * @param data Pointer to the data storage.
 * @param writeLength Length of data to write.
 * @param readLength Length of data to read.
 *
 * @return false - UART not implemented
 */
bool tmc5240_readWriteUART(uint16_t icID, uint8_t *data, size_t writeLength,
			   size_t readLength)
{
	return false; /* Not needed in SPI implementation */
}

/**
 * @brief Function that returns the type of node address of TMC IC.
 *
 * This function returns the type of node address of TMC IC.
 *
 * @param icID ID of the IC.
 *
 * @return uint8_t
 */
uint8_t tmc5240_getNodeAddress(uint16_t icID)
{
	return 0; /* Not needed in SPI implementation */
}

/**
 * @brief Function that returns the type of bus used by the TMC IC.
 *
 * This function returns the type of bus used by the TMC IC.
 *
 * @param icID ID of the IC.
 *
 * @return TMC5240BusType
 */
TMC5240BusType tmc5240_getBusType(uint16_t icID)
{
	return IC_BUS_SPI; /* Assuming SPI for this implementation */
}

/**
 * @brief TMC-API callback for reset operation
 *
 * This function is required by TMC-API but not used in basic SPI implementation.
 * Reserved for future use.
 *
 * @return 1 (success)
 */
uint8_t reset(void)
{
	return 1;
}

/**
 * @brief TMC-API callback for restore operation
 *
 * This function is required by TMC-API but not used in basic SPI implementation.
 * Reserved for future use.
 *
 * @return 1 (success)
 */
uint8_t restore(void)
{
	return 1;
}

/**
 * @brief Initialize the TMC5240 device.
 * @param device - TMC5240 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		       TMC5240 device to be initialized.
 * @return 0 in case of success, negative error code otherwise
*/
int tmc5240_init(struct tmc5240_dev **device,
		 struct tmc5240_init_param *init_param)
{
	struct tmc5240_dev *dev;
	int ret;
	uint32_t reg_val;
	int id;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct tmc5240_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* Get a free device ID for multi-instance support */
	id = tmc5240_get_free_id();
	if (id < 0) {
		ret = id;
		goto err_free;
	}

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init_param);
	if (ret)
		goto err_free;

	/* Assign device ID and register in lookup table */
	dev->id = (uint8_t)id;
	tmc5240_devices[id] = dev;

	dev->clock = init_param->clock;

	/* Configure driver settings */
	reg_val = no_os_field_prep(TMC5240_SLOPE_CONTROL_MASK,
				   init_param->slope_control) |
		  no_os_field_prep(TMC5240_CURRENT_RANGE_MASK,
				   init_param->current_range);
	ret = tmc5240_write(dev, TMC5240_DRV_CONF, reg_val);
	if (ret)
		goto err_remove;

	/* Motor Current configurations */
	ret = tmc5240_write(dev, TMC5240_GLOBAL_SCALER, init_param->global_scaler);
	if (ret)
		goto err_remove;

	reg_val = no_os_field_prep(TMC5240_IRUNDELAY_MASK,
				   init_param->irundelay) |
		  no_os_field_prep(TMC5240_IHOLDDELAY_MASK,
				   init_param->iholddelay) |
		  no_os_field_prep(TMC5240_IRUN_MASK,
				   init_param->current_run) |
		  no_os_field_prep(TMC5240_IHOLD_MASK,
				   init_param->current_hold);
	ret = tmc5240_write(dev, TMC5240_IHOLD_IRUN, reg_val);
	if (ret)
		goto err_remove;

	/* Set Microstepping Resolution */
	ret = tmc5240_read(dev, TMC5240_CHOPCONF, &reg_val);
	if (ret)
		goto err_remove;

	dev->microsteps_res = init_param->microsteps_res;

	reg_val &= ~TMC5240_TOFF_MASK & ~TMC5240_TBL_MASK & ~TMC5240_MRES_MASK;
	reg_val |= no_os_field_prep(TMC5240_TOFF_MASK, init_param->toff) |
		   no_os_field_prep(TMC5240_TBL_MASK, init_param->tbl) |
		   no_os_field_prep(TMC5240_MRES_MASK, init_param->microsteps_res);

	ret = tmc5240_write(dev, TMC5240_CHOPCONF, reg_val);
	if (ret)
		goto err_remove;

	dev->step_angle_millidegrees = init_param->step_angle_millidegrees;

	/* Set ramp mode to position */
	ret = tmc5240_write(dev, TMC5240_RAMPMODE, TMC5240_MODE_POSITION);
	if (ret)
		goto err_remove;

	/* Configure motion profile parameters */
	dev->amax = init_param->amax;
	ret = tmc5240_write(dev, TMC5240_AMAX, dev->amax);
	if (ret)
		goto err_remove;

	dev->vmax = init_param->vmax;
	ret = tmc5240_write(dev, TMC5240_VMAX, dev->vmax);
	if (ret)
		goto err_remove;

	dev->dmax = init_param->dmax;
	ret = tmc5240_write(dev, TMC5240_DMAX, dev->dmax);
	if (ret)
		goto err_remove;

	*device = dev;

	return 0;

err_remove:
	/* Remove device from lookup table */
	tmc5240_devices[id] = NULL;
	/* Remove SPI descriptor */
	if (dev->spi_desc)
		no_os_spi_remove(dev->spi_desc);
err_free:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by the tmc5240_init()
 * @param device - TMC5240 device descriptor
 *
 * @return 0 in case of success, negative error code otherwise
*/
int tmc5240_remove(struct tmc5240_dev *device)
{
	int ret = 0;

	if (!device)
		return -ENODEV;

	/* Remove device from lookup table */
	if (device->id < TMC5240_MAX_DEVICES)
		tmc5240_devices[device->id] = NULL;

	if (device->spi_desc) {
		ret = no_os_spi_remove(device->spi_desc);
		device->spi_desc = NULL;
	}

	no_os_free(device);

	return ret;
}

/**
 * @brief Reads data from a given TMC5240 Register.
 *
 * This function wraps the TMC-API readRegister function to read data from a
 * specified register of the TMC5240 device.
 *
 * @param device - The device structure.
 * @param reg_addr - Address of the register.
 * @param reg_data - Stores data read from the register.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_read(struct tmc5240_dev *device, uint8_t reg_addr,
		 uint32_t *reg_data)
{
	if (!device)
		return -ENODEV;

	if (!reg_data)
		return -EINVAL;

	*reg_data = tmc5240_readRegister(device->id, reg_addr);

	return 0;
}

/**
 * @brief Writes data to a given TMC5240 Register.
 *
 * This function wraps the TMC-API writeRegister function to write data to a
 * specified register of the TMC5240 device.
 *
 * @param device - The device structure.
 * @param reg_addr - Address of the register to write to.
 * @param reg_data - 32-bit data value to write to the register.
 *
 * @return 0 in case of success, negative error code otherwise.
 *
 * @note The TMC-API writeRegister function does not return error codes.
 *       Any SPI communication errors are logged in tmc5240_readWriteSPI()
 *       but cannot be propagated back through this function. This is a
 *       limitation of the TMC-API callback interface.
 */
int tmc5240_write(struct tmc5240_dev *device, uint8_t reg_addr,
		  uint32_t reg_data)
{
	if (!device)
		return -ENODEV;

	tmc5240_writeRegister(device->id, reg_addr, (int32_t)reg_data);

	return 0;
}

/**
 * @brief Set the target position for the TMC5240 motor driver.
 *
 * This function configures the TMC5240 to move to a specified target position
 * in position mode. It ensures that the acceleration (AMAX) and maximum velocity
 * (VMAX) parameters are properly configured before setting the target. If either
 * AMAX or VMAX is zero, the function restores them to the device's stored default
 * values to enable motion. The motor will ramp up to VMAX, move toward the target,
 * and decelerate according to the configured motion profile.
 *
 * @param device - Pointer to the TMC5240 device structure.
 * @param position - Target position value to move to in microsteps.
 *
 * @return 0 on success, negative error code on failure.
 */
int tmc5240_set_target_pos(struct tmc5240_dev *device, int32_t position)
{
	int ret;
	uint32_t amax;
	uint32_t vmax;

	ret = tmc5240_write(device, TMC5240_RAMPMODE, TMC5240_MODE_POSITION);
	if (ret)
		return ret;

	ret = tmc5240_read(device, TMC5240_AMAX, &amax);
	if (ret)
		return ret;

	if (amax == 0) {
		ret = tmc5240_write(device, TMC5240_AMAX, device->amax);
		if (ret)
			return ret;
	}

	ret = tmc5240_read(device, TMC5240_VMAX, &vmax);
	if (ret)
		return ret;

	if (vmax == 0) {
		ret = tmc5240_write(device, TMC5240_VMAX, device->vmax);
		if (ret)
			return ret;
	}

	ret = tmc5240_write(device, TMC5240_XTARGET, (uint32_t)position);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the target position of the motor.
 *
 * This function reads the target position register (XTARGET) from the TMC5240
 * device and stores the result in the provided position pointer. The target position
 * represents the desired destination when operating in position mode.
 *
 * @param device - Pointer to the TMC5240 device structure.
 * @param position - Pointer to store the target position value in microsteps.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_get_target_pos(struct tmc5240_dev *device, int32_t *position)
{
	uint32_t temp;
	int ret;

	ret = tmc5240_read(device, TMC5240_XTARGET, &temp);
	if (ret)
		return ret;

	*position = (int32_t)temp;

	return 0;
}

/**
 * @brief Replace the current position of the motor with a new absolute position value.
 *
 * This function sets the motor's current position register (XACTUAL) to a specified value.
 * Before updating the position, it switches the motor to hold mode (RAMPMODE = MODE_HOLD)
 * to ensure the position can be safely updated without triggering unwanted motion.
 * This is useful for rehoming or synchronizing the motor's internal position counter
 * with an external reference system.
 *
 * @param device - Pointer to the TMC5240 device structure.
 * @param position - The new position value to set in microsteps.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_set_current_pos(struct tmc5240_dev *device, int32_t position)
{
	int ret;

	ret = tmc5240_write(device, TMC5240_RAMPMODE, TMC5240_MODE_HOLD);
	if (ret)
		return ret;

	ret = tmc5240_write(device, TMC5240_XACTUAL, (uint32_t)position);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the current position of the motor.
 *
 * This function reads the actual position register (XACTUAL) from the TMC5240
 * device and stores the result in the provided position pointer.
 *
 * @param device   - Pointer to the TMC5240 device structure.
 * @param position - Pointer to store the current motor position value.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_get_current_pos(struct tmc5240_dev *device, int32_t *position)
{
	uint32_t temp;
	int ret;

	ret = tmc5240_read(device, TMC5240_XACTUAL, &temp);
	if (ret)
		return ret;

	*position = (int32_t)temp;

	return 0;
}

/**
 * @brief Get the current acceleration of the motor.
 *
 * This function reads the actual acceleration value from the TMC5240 device.
 *
 * @param device - Pointer to the TMC5240 device structure.
 * @param acceleration - Pointer to store the read acceleration value.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_get_current_accel(struct tmc5240_dev *device, int32_t *acceleration)
{
	uint32_t temp;
	int ret;

	ret = tmc5240_read(device, TMC5240_AACTUAL, &temp);
	if (ret)
		return ret;

	*acceleration = no_os_sign_extend32(temp, 23);

	return 0;
}

/**
 * @brief Set the target velocity for the TMC5240 motor driver.
 *
 * This function sets the target velocity for the motor by configuring the VMAX
 * register and the appropriate ramp mode (positive or negative velocity). If the
 * maximum acceleration (AMAX) is zero, it will be set to the device's default
 * value before setting the velocity.
 *
 * @param device - Pointer to the TMC5240 device structure.
 * @param velocity - Target velocity value. Positive values result in positive
 *                   velocity ramp mode, negative values result in negative
 *                   velocity ramp mode. The absolute value is written to VMAX.
 *
 * @return 0 on success, negative error code otherwise.
 */

int tmc5240_set_target_vel(struct tmc5240_dev *device, int32_t velocity)
{
	int ret;
	uint32_t amax;
	uint32_t vmax_abs;

	ret = tmc5240_read(device, TMC5240_AMAX, &amax);
	if (ret)
		return ret;

	if (amax == 0) {
		ret = tmc5240_write(device, TMC5240_AMAX, device->amax);
		if (ret)
			return ret;
	}

	vmax_abs = (velocity < 0) ? (uint32_t)(-velocity) : (uint32_t)velocity;

	if (vmax_abs > TMC5240_VMAX_MASK)
		return -EINVAL;

	if (velocity < 0) {
		ret = tmc5240_write(device, TMC5240_VMAX, vmax_abs);
		if (ret)
			return ret;

		ret = tmc5240_write(device, TMC5240_RAMPMODE,
				    TMC5240_MODE_VELNEG);
	} else {
		ret = tmc5240_write(device, TMC5240_VMAX, vmax_abs);
		if (ret)
			return ret;

		ret = tmc5240_write(device, TMC5240_RAMPMODE,
				    TMC5240_MODE_VELPOS);
	}

	return ret;
}

/**
 * @brief Get the current velocity of the motor.
 *
 * This function reads the actual velocity register (VACTUAL) from the TMC5240
 * device and returns the velocity value as a signed 24-bit integer converted
 * to 32-bit format.
 *
 * @param device - Pointer to the TMC5240 device structure.
 * @param velocity - Pointer to store the current velocity value.
 *                   The value is sign-extended from 24-bit to 32-bit.
 *
 * @return 0 on success, negative error code otherwise.
 */
int tmc5240_get_current_vel(struct tmc5240_dev *device, int32_t *velocity)
{
	int ret;
	uint32_t reg_val;

	ret = tmc5240_read(device, TMC5240_VACTUAL, &reg_val);
	if (ret)
		return ret;

	*velocity = no_os_sign_extend32(reg_val, 23);

	return 0;
}

/**
 * @brief Get the shaft direction setting from the TMC5240 device.
 *
 * This function reads the GCONF register and extracts the shaft direction bit.
 * The shaft direction determines whether the motor rotates in normal or
 * inverted direction.
 *
 * @param device - Pointer to the TMC5240 device structure
 * @param dir - Pointer to store the shaft direction value:
 * 		- 0: Normal direction
 * 		- 1: Reverse direction
 * @return 0 on success, negative error code otherwise
 */
int tmc5240_get_shaft_dir(struct tmc5240_dev *device, uint8_t *dir)
{
	int ret;
	uint32_t reg_val;

	ret = tmc5240_read(device, TMC5240_GCONF, &reg_val);
	if (ret)
		return ret;

	*dir = no_os_field_get(TMC5240_SHAFT_MASK, reg_val);

	return 0;
}

/**
 * @brief Set the shaft direction for the TMC5240 device.
 *
 * This function modifies the GCONF register to set the shaft direction bit.
 * Setting the shaft direction allows reversing the motor rotation direction
 * without changing the wiring.
 *
 * @param device - Pointer to the TMC5240 device structure
 * @param dir - Shaft direction value to set:
 *		- 0: Normal direction
 * 		- 1: Reverse direction
 * @return 0 on success, negative error code otherwise
 */
int tmc5240_set_shaft_dir(struct tmc5240_dev *device, uint8_t dir)
{
	int ret;
	uint32_t reg_val;

	ret = tmc5240_read(device, TMC5240_GCONF, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~TMC5240_SHAFT_MASK;
	reg_val |= no_os_field_prep(TMC5240_SHAFT_MASK, dir);

	ret = tmc5240_write(device, TMC5240_GCONF, reg_val);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the maximum acceleration (AMAX) value.
 * @param device - Pointer to the TMC5240 device structure.
 * @param amax - Pointer to store the retrieved AMAX value.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_get_amax(struct tmc5240_dev *device, int32_t *amax)
{
	uint32_t temp;
	int ret;

	ret = tmc5240_read(device, TMC5240_AMAX, &temp);
	if (ret)
		return ret;

	*amax = (int32_t)temp;

	return 0;
}

/**
 * @brief Set the maximum acceleration (AMAX) value.
 * @param device - Pointer to the TMC5240 device structure.
 * @param amax - The AMAX value to set.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_set_amax(struct tmc5240_dev *device, int32_t amax)
{
	return tmc5240_write(device, TMC5240_AMAX, (uint32_t)amax);
}

/**
 * @brief Get the maximum velocity (VMAX) value.
 * @param device - Pointer to the TMC5240 device structure.
 * @param vmax - Pointer to store the retrieved VMAX value.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_get_vmax(struct tmc5240_dev *device, int32_t *vmax)
{
	uint32_t temp;
	int ret;

	ret = tmc5240_read(device, TMC5240_VMAX, &temp);
	if (ret)
		return ret;

	*vmax = (int32_t)temp;

	return 0;
}

/**
 * @brief Set the maximum velocity (VMAX) value.
 * @param device - Pointer to the TMC5240 device structure.
 * @param vmax - The VMAX value to set.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_set_vmax(struct tmc5240_dev *device, int32_t vmax)
{
	return tmc5240_write(device, TMC5240_VMAX,
			     (uint32_t)(vmax & TMC5240_VMAX_MASK));
}

/**
 * @brief Get the maximum deceleration (DMAX) value.
 * @param device - Pointer to the TMC5240 device structure.
 * @param dmax - Pointer to store the retrieved DMAX value.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_get_dmax(struct tmc5240_dev *device, int32_t *dmax)
{
	uint32_t temp;
	int ret;

	ret = tmc5240_read(device, TMC5240_DMAX, &temp);
	if (ret)
		return ret;

	*dmax = (int32_t)temp;

	return 0;
}

/**
 * @brief Set the maximum deceleration (DMAX) value.
 * @param device - Pointer to the TMC5240 device structure.
 * @param dmax - The DMAX value to set.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_set_dmax(struct tmc5240_dev *device, int32_t dmax)
{
	return tmc5240_write(device, TMC5240_DMAX,
			     (uint32_t)(dmax & TMC5240_DMAX_MASK));
}

/**
 * @brief Get the ramp mode configuration.
 * @param device - Pointer to the TMC5240 device structure.
 * @param rampmode - Pointer to store the retrieved ramp mode value.
 *                   Possible values:
 *                   - 0: Position mode
 *                   - 1: Positive velocity mode
 *                   - 2: Negative velocity mode
 *                   - 3: Hold mode
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_get_rampmode(struct tmc5240_dev *device, int32_t *rampmode)
{
	uint32_t temp;
	int ret;

	ret = tmc5240_read(device, TMC5240_RAMPMODE, &temp);
	if (ret)
		return ret;

	*rampmode = (int32_t)(temp & TMC5240_RAMPMODE_MASK);

	return 0;
}

/**
 * @brief Set the ramp mode configuration.
 * @param device - Pointer to the TMC5240 device structure.
 * @param rampmode - The ramp mode value to set.
 *                   Possible values:
 *                   - 0: Position mode
 *                   - 1: Positive velocity mode
 *                   - 2: Negative velocity mode
 *                   - 3: Hold mode
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_set_rampmode(struct tmc5240_dev *device, int32_t rampmode)
{
	return tmc5240_write(device, TMC5240_RAMPMODE,
			     (uint32_t)(rampmode & TMC5240_RAMPMODE_MASK));
}

/**
 * @brief Stop the motor by setting VMAX to 0.
 * @param device - Pointer to the TMC5240 device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_stop(struct tmc5240_dev *device)
{
	return tmc5240_write(device, TMC5240_VMAX, 0);
}
