/***************************************************************************//**
 *   @file   max9611.h
 *   @brief  Header file of max9611 Family Driver.
 *   @author MSosa (marcpaolo.sosa@analog.com)
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef __MAX9611_H__
#define __MAX9611_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_delay.h"

#define MAX9611_MUX_MASK		NO_OS_GENMASK(2, 0)
#define MAX9611_MODE_MASK		NO_OS_GENMASK(7, 5)
#define MAX9611_RAW_DATA_MASK	NO_OS_GENMASK(15, 4)

#define MAX9611_SHDN_MASK		NO_OS_BIT(3)
#define MAX9611_LR_MASK			NO_OS_BIT(4)
#define MAX9611_DTIME_MASK		NO_OS_BIT(3)
#define MAX9611_RTIME_MASK		NO_OS_BIT(2)

#define	MAX9611_CSA_MSB			0x00
#define MAX9611_CSA_LSB			0x01
#define	MAX9611_RSP_MSB			0x02
#define	MAX9611_RSP_LSB			0x03
#define	MAX9611_OUT_MSB			0x04
#define	MAX9611_OUT_LSB			0x05
#define	MAX9611_SET_MSB			0x06
#define	MAX9611_SET_LSB			0x07
#define	MAX9611_TMP_MSB			0x08
#define	MAX9611_TMP_LSB			0x09
#define	MAX9611_CTR1			0x0A
#define	MAX9611_CTR2			0x0B

#define MAX9611_OUT_OF_BOUNDS  	0x0C

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum max9611_data {
	MAX9611_DATA_CSA,
	MAX9611_DATA_RSP,
	MAX9611_DATA_OUT,
	MAX9611_DATA_SET,
	MAX9611_DATA_TMP,
};

enum max9611_addr_lvls {
	MAX9611_ZERO_VCC,
	MAX9611_33_VCC,
	MAX9611_66_VCC,
	MAX9611_100_VCC,
};

enum max9611_delay_time {
	MAX9611_1MS,
	MAX9611_100US,
};

enum max9611_retry_time {
	MAX9611_50MS,
	MAX9611_10MS,
};

enum max9611_mux_conf {
	MAX9611_CONF_SENSE_1X,
	MAX9611_CONF_SENSE_4X,
	MAX9611_CONF_SENSE_8X,
	MAX9611_CONF_IN_COM_MODE,
	MAX9611_CONF_OUT_ADC,
	MAX9611_CONF_SET_ADC,
	MAX9611_CONF_TEMP,
	MAX9611_FAST_MODE,
};

enum max9611_mode_conf {
	MAX9611_NORMAL_MODE = 0x0,
	MAX9611_OPAMP_MODE = 0x3,
	MAX9611_COMPARATOR_MODE = 0x7,
};

struct max9611_dev {
	struct no_os_i2c_desc *i2c_desc;
	/* ADC Data configuration */
	enum max9611_data capture_type;
};

struct max9611_init_param {
	struct no_os_i2c_init_param i2c_init;
};

/* Generate slave address depending on A0 and A1 logic levels */
int max9611_addr_gen(struct max9611_init_param *device_ip,
		     enum max9611_addr_lvls a0, enum max9611_addr_lvls a1);

/* Initialize device */
int max9611_init(struct max9611_dev **device,
		 struct max9611_init_param init_param);

/* Deallocate resources for device */
int max9611_remove(struct max9611_dev *dev);

/* Read byte/s from selected starting register */
int max9611_read(struct max9611_dev *dev, uint8_t addr,
		 uint8_t bytes, uint8_t *read_data);

/* Write byte/s to selected starting register */
int max9611_write(struct max9611_dev *dev, uint8_t addr,
		  uint8_t bytes, uint8_t *write_data);

/* Update selected register using mask and desired value */
int max9611_reg_update(struct max9611_dev *dev, uint8_t addr,
		       int update_mask, int update_val);

/* Set MAX9611 operating mode */
int max9611_set_mode(struct max9611_dev *dev, enum max9611_mode_conf mode);

/* Get MAX9611 operating mode */
int max9611_get_mode(struct max9611_dev *dev, enum max9611_mode_conf *mode);

/* Set MAX9611 MUX configuration */
int max9611_set_mux(struct max9611_dev *dev, enum max9611_mux_conf mux);

/* Get MAX9611 MUX configuration */
int max9611_get_mux(struct max9611_dev *dev, enum max9611_mux_conf *mux);

/* Configure MAX9611 for shutdown or power up */
int max9611_shutdown(struct max9611_dev *dev, bool is_shdn);

/* Set MAX9611 LR bit */
int max9611_set_lr(struct max9611_dev *dev, bool is_normal);

/* Get MAX9611 LR bit */
int max9611_get_lr(struct max9611_dev *dev, bool *lr);

/* Set MAX9611 delay time */
int max9611_set_delay(struct max9611_dev *dev, enum max9611_delay_time dtime);

/* Get MAX9611 delay time */
int max9611_get_delay(struct max9611_dev *dev, enum max9611_delay_time *dtime);

/* Set MAX9611 retry time */
int max9611_set_retry(struct max9611_dev *dev, enum max9611_retry_time rtime);

/* Get MAX9611 retry time */
int max9611_get_retry(struct max9611_dev *dev, enum max9611_retry_time *rtime);

/* Get MAX9611 raw data */
int max9611_get_raw(struct max9611_dev *dev, uint16_t *raw);

#endif	/* __MAX9611_H__ */
