/***************************************************************************//**
 *   @file   tmc5240_control.c
 *   @brief  Functions source for TMC control
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "TMC5240.h"
#include "tmc5240_wrapper.h"

struct no_os_spi_desc *tmc_spi_desc;

/**
 * @brief Read/Write SPI function for TMC-API
 *
 * This function is used to read and write data to the TMC ICs via SPI.
 *
 * @param icID ID of the IC.
 * @param data Pointer to the data storage.
 * @param dataLength Length of data to transfer.
 *
 * @return none
 */
void tmc5240_readWriteSPI(uint16_t icID, uint8_t *data, size_t dataLength)
{
	no_os_spi_write_and_read(tmc_spi_desc, data, dataLength);

}

/**
 * @brief Read/Write UART function for TMC-API
 *
 * This function is used to read and write data to the TMC ICs via UART.
 *
 * @param icID ID of the IC.
 * @param data Pointer to the data storage.
 * @param writeLength Length of data to write.
 * @param readLength Length of data to read.
 *
 * @return none
 */
bool tmc5240_readWriteUART(uint16_t icID, uint8_t *data, size_t writeLength,
			   size_t readLength)
{
	return false; // Not needed in SPI implementation
}

/**
 * @brief Function the returns the type of node address of TMC IC.
 *
 * Function the returns the type of node address of TMC IC.
 *
 * @param icID ID of the IC.
 *
 * @return uint8_t
 */
uint8_t tmc5240_getNodeAddress(uint16_t icID)
{
	return 0; // Not needed in SPI implementation
}

/**
 * @brief Function the returns the type of bus used by the TMC IC.
 *
 * This function returns the type of bus used by the TMC IC.
 *
 * @param icID ID of the IC.
 *
 * @return TMC5240BusType
 */
TMC5240BusType tmc5240_getBusType(uint16_t icID)
{
	return IC_BUS_SPI; // Assuming SPI for this implementation
}

/**
 * @brief Initialize the TMC5240 device.
 * @param device - TMC5240 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		       TMC5240 device to be initialized.
 * @return 0 in case of succes, negative error code otherwise
*/
int tmc5240_init(struct tmc5240_dev **device,
		 struct tmc5240_init_param *init_param)
{
	struct tmc5240_dev *dev;
	int ret;
	uint8_t i;

	dev = (struct tmc5240_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init_param);
	if (ret)
		goto err;

	tmc_spi_desc = dev->spi_desc;

	dev->amax = init_param->amax;
	dev->vmax = init_param->vmax;

	/* Initial Config */
	tmc5240_write(dev, TMC5240_GCONF, 0x00000000);
	tmc5240_write(dev, TMC5240_DRV_CONF, 0x00000003);
	tmc5240_write(dev, TMC5240_GLOBAL_SCALER, 0x000000FB);

	/* Motor Current configurations */
	tmc5240_write(dev, TMC5240_IHOLD_IRUN, 0x04041000);
	tmc5240_write(dev, TMC5240_CHOPCONF, 0x10410153);
	tmc5240_write(dev, TMC5240_RAMPMODE, 0x00000000);

	/* Motor Motion Parameters  (default stationary) */
	for (i = 0; i < 3; i++) {
		tmc5240_write(dev, TMC5240_AMAX, 0);
		tmc5240_write(dev, TMC5240_VMAX, 0);
	}

	/* Enable Motor (default disable) */
	tmc5240_write(dev, TMC5240_GCONF, 0x00000002);

	/* Get initial position */
	//ret = tmc5240_read(dev, TMC5240_XTARGET);

	*device = dev;

	return 0;

err:
	tmc5240_remove(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by the tmc5240_init()
 * @param desc - TMC5240 device descriptor
 * @return- 0 in case of succes, negative error code otherwise
*/
int tmc5240_remove(struct tmc5240_dev *device)
{
	int ret;

	if (!device)
		return -ENODEV;

	if (device->spi_desc) {
		ret = no_os_spi_remove(device->spi_desc);
		if (ret)
			return ret;
		device->spi_desc = NULL;
	}

	no_os_free(device);

	return 0;
}

/**
 * @brief Reads data from a given TMC5240 Register.
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
	*reg_data = tmc5240_readRegister(0, reg_addr);

	return 0;
}

/**
 * @brief Writes data to a given TMC5240 Register.
 *
 * @param device - The device structure.
 * @param reg_addr - Address of the register.
 * @param reg_data - Data to write to the register.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int tmc5240_write(struct tmc5240_dev *device, uint8_t reg_addr,
		  uint32_t reg_data)
{
	tmc5240_writeRegister(0, reg_addr, (int32_t)reg_data);

	return 0;
}

int tmc5240_set_target_pos(struct tmc5240_dev *device, int32_t position,
			   uint32_t velocityMax)
{
	int ret;

	ret = tmc5240_write(device, TMC5240_RAMPMODE, TMC5240_MODE_POSITION);
	if (ret)
		return ret;

	ret = tmc5240_write(device, TMC5240_VMAX, velocityMax);
	if (ret)
		return ret;

	ret = tmc5240_write(device, TMC5240_XTARGET, position);
	if (ret)
		return ret;

	return 0;
}

int tmc5240_get_target_pos(struct tmc5240_dev *device, int32_t *position)
{
	return tmc5240_read(device, TMC5240_XTARGET, position);
}

int tmc5240_get_current_pos(struct tmc5240_dev *device, int32_t *position)
{
	return tmc5240_read(device, TMC5240_XACTUAL, position);
}

int tmc5240_get_amax(struct tmc5240_dev *device, int32_t *amax)
{
	return tmc5240_read(device, TMC5240_AMAX, amax);
}

int tmc5240_set_amax(struct tmc5240_dev *device, int32_t amax)
{
	return tmc5240_write(device, TMC5240_AMAX, amax);
}

int tmc5240_get_vmax(struct tmc5240_dev *device, int32_t *vmax)
{
	return tmc5240_read(device, TMC5240_VMAX, vmax);
}

int tmc5240_set_vmax(struct tmc5240_dev *device, int32_t vmax)
{
	return tmc5240_write(device, TMC5240_VMAX, vmax);
}

int tmc5240_get_dmax(struct tmc5240_dev *device, int32_t *dmax)
{
	return tmc5240_read(device, TMC5240_DMAX, dmax);
}

int tmc5240_set_dmax(struct tmc5240_dev *device, int32_t dmax)
{
	return tmc5240_write(device, TMC5240_DMAX, dmax);
}

int tmc5240_get_ramp_mode(struct tmc5240_dev *device, int32_t *ramp_mode)
{
	return tmc5240_read(device, TMC5240_RAMPMODE, ramp_mode);
}

int tmc5240_set_ramp_mode(struct tmc5240_dev *device, int32_t ramp_mode)
{
	return tmc5240_write(device, TMC5240_RAMPMODE, ramp_mode);
}

int tmc5240_stop(struct tmc5240_dev *device)
{
	return tmc5240_write(device, TMC5240_VMAX, 0);
}