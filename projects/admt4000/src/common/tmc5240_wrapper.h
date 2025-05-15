/***************************************************************************//**
 *   @file   iio_admtTMC5240.h
 *   @brief  Header file of IIO TMC5240 driver.
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

#ifndef TMC5240_WRAPPER_H
#define TMC5240_WRAPPER_H

#include <stdint.h>
#include <string.h>
#include "no_os_spi.h"
#include "no_os_util.h"


enum tmc5240_ramp_mode {
	TMC5240_RAMP_MODE_POSITION = 0,
	TMC5240_RAMP_MODE_VPOSITIVE = 1,
	TMC5240_RAMP_MODE_VNEGATIVE = 2,
	TMC5240_RAMP_MODE_HOLD = 3,
};

struct tmc5240_dev {
	struct no_os_spi_desc *spi_desc;
	uint32_t amax;
	uint32_t vmax;
};

struct tmc5240_init_param {
	struct no_os_spi_init_param spi_init_param;
	uint32_t amax;
	uint32_t vmax;
};

/**
 * @brief Initialize the TMC5240 device.
 * @param device - TMC5240 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		       TMC5240 device to be initialized.
 * @return 0 in case of succes, negative error code otherwise
*/
int tmc5240_init(struct tmc5240_dev **device,
		 struct tmc5240_init_param *init_param);

/**
 * @brief Free the resources allocated by the tmc5240_init()
 * @param desc - TMC5240 device descriptor
 * @return- 0 in case of succes, negative error code otherwise
*/
int tmc5240_remove(struct tmc5240_dev *device);

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
		 uint32_t *reg_data);

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
		  uint32_t reg_data);

int tmc5240_set_target_pos(struct tmc5240_dev *device, int32_t position,
			   uint32_t velocityMax);

int tmc5240_get_target_pos(struct tmc5240_dev *device, int32_t *position);

int tmc5240_get_current_pos(struct tmc5240_dev *device, int32_t *position);

int tmc5240_get_amax(struct tmc5240_dev *device, int32_t *amax);

int tmc5240_set_amax(struct tmc5240_dev *device, int32_t amax);

int tmc5240_get_vmax(struct tmc5240_dev *device, int32_t *vmax);

int tmc5240_set_vmax(struct tmc5240_dev *device, int32_t vmax);

int tmc5240_get_dmax(struct tmc5240_dev *device, int32_t *dmax);

int tmc5240_set_dmax(struct tmc5240_dev *device, int32_t dmax);

int tmc5240_get_ramp_mode(struct tmc5240_dev *device, int32_t *ramp_mode);

int tmc5240_set_ramp_mode(struct tmc5240_dev *device, int32_t  ramp_mode);

int tmc5240_stop(struct tmc5240_dev *device);

#endif
