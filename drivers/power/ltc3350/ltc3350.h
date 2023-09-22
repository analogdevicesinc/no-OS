/***************************************************************************//**
*   @file   ltc3350.c
*   @brief  Implementation of LTC3350 Driver
*   @authors Sankalp Khandkar (sankalp.khandkar@analog.com)
*            Ignacio Rojas (ignacio.rojas@analog.com)
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

#ifndef __LTC3350_H__
#define __LTC3350_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* LTC3350 Register Map */
#define LTC3350_AD_CLR_ALARMS                   0x00
#define LTC3350_AD_MSK_ALARMS                   0x01
#define LTC3350_AD_MSK_MON_STATUS               0x02
#define LTC3350_AD_CAP_ESR_PER                  0x04
#define LTC3350_AD_VCAPFB_DAC                   0x05
#define LTC3350_AD_VSHUNT                       0x06
#define LTC3350_AD_CAP_UV_LVL                   0x07
#define LTC3350_AD_CAP_OV_LVL                   0x08
#define LTC3350_AD_GPI_UV_LVL                   0x09
#define LTC3350_AD_GPI_OV_LVL                   0x0A
#define LTC3350_AD_VIN_UV_LVL                   0x0B
#define LTC3350_AD_VIN_OV_LVL                   0x0C
#define LTC3350_AD_VCAP_UV_LVL                  0x0D
#define LTC3350_AD_VCAP_OV_LVL                  0x0E
#define LTC3350_AD_VOUT_UV_LVL                  0x0F
#define LTC3350_AD_VOUT_OV_LVL                  0x10
#define LTC3350_AD_IIN_OC_LVL                   0x11
#define LTC3350_AD_ICHG_UC_LVL                  0x12
#define LTC3350_AD_DTEMP_COLD_LVL               0x13
#define LTC3350_AD_DTEMP_HOT_LVL                0x14
#define LTC3350_AD_ESR_HI_LVL                   0x15
#define LTC3350_AD_CAP_LO_LVL                   0x16
#define LTC3350_AD_CTL_REG                      0x17
#define LTC3350_AD_NUM_CAPS                     0x1A
#define LTC3350_AD_CHRG_STATUS                  0x1B
#define LTC3350_AD_MON_STATUS                   0x1C
#define LTC3350_AD_ALARM_REG                    0x1D
#define LTC3350_AD_MEAS_CAP                     0x1E
#define LTC3350_AD_MEAS_ESR                     0x1F
#define LTC3350_AD_MEAS_VCAP1                   0x20
#define LTC3350_AD_MEAS_VCAP2                   0x21
#define LTC3350_AD_MEAS_VCAP3                   0x22
#define LTC3350_AD_MEAS_VCAP4                   0x23
#define LTC3350_AD_MEAS_GPI                     0x24
#define LTC3350_AD_MEAS_VIN                     0x25
#define LTC3350_AD_MEAS_VCAP                    0x26
#define LTC3350_AD_MEAS_VOUT                    0x27
#define LTC3350_AD_MEAS_IIN                     0x28
#define LTC3350_AD_MEAS_ICHG                    0x29
#define LTC3350_AD_MEAS_DTEMP                   0x2A

/******************************************************************************/
/******************* Driver pre-compile time settings *************************/
/******************************************************************************/

/**
 * @brief   LTC3350 measurements configuration switch.
 * @details If set to @p TRUE more configurations are available.
 * @note    The default is @p TRUE.
 */
#ifndef LTC3350_USE_MEASUREMENTS
#define LTC3350_USE_MEASUREMENTS               	1
#endif

/**
 * @brief   LTC3350 alarms configuration switch.
 * @details If set to @p TRUE more configurations are available.
 * @note    The default is @p FALSE.
 */
#ifndef LTC3350_USE_ALARMS
#define LTC3350_USE_ALARMS               	0
#endif

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/*LTC3350 alarms*/
enum ltc3350_enum_alarms {
	LTC3350_CAP_UV_LVL            = 0,
	LTC3350_CAP_OV_LVL            = 1,
	LTC3350_GPI_UV_LVL            = 2,
	LTC3350_GPI_OV_LVL            = 3,
	LTC3350_VIN_UV_LVL            = 4,
	LTC3350_VIN_OV_LVL            = 5,
	LTC3350_VCAP_UV_LVL           = 6,
	LTC3350_VCAP_OV_LVL           = 7,
	LTC3350_VOUT_UV_LVL           = 8,
	LTC3350_VOUT_OV_LVL           = 9,
	LTC3350_IIN_OC_LVL            = 10,
	LTC3350_ICHG_UC_LVL           = 11,
	LTC3350_DTEMP_COLD_LVL        = 12,
	LTC3350_DTEMP_HOT_LVL         = 13,
	LTC3350_ESR_HI_LVL            = 14,
	LTC3350_CAP_LO_LVL            = 15
};

/*LTC3350 alarms_mask*/
enum ltc3350_enum_alarms_mask {
	LTC3350_CAP_UV_LVL_B_BIT      = NO_OS_BIT(0),
	LTC3350_CAP_OV_LVL_BIT        = NO_OS_BIT(1),
	LTC3350_GPI_UV_LVL_BIT        = NO_OS_BIT(2),
	LTC3350_GPI_OV_LVL_BIT        = NO_OS_BIT(3),
	LTC3350_VIN_UV_LVL_BIT        = NO_OS_BIT(4),
	LTC3350_VIN_OV_LVL_BIT        = NO_OS_BIT(5),
	LTC3350_VCAP_UV_LVL_BIT       = NO_OS_BIT(6),
	LTC3350_VCAP_OV_LVL_BIT       = NO_OS_BIT(7),
	LTC3350_VOUT_UV_LVL_BIT       = NO_OS_BIT(8),
	LTC3350_VOUT_OV_LVL_BIT       = NO_OS_BIT(9),
	LTC3350_IIN_OC_LVL_BIT        = NO_OS_BIT(10),
	LTC3350_ICHG_UC_LVL_BIT       = NO_OS_BIT(11),
	LTC3350_DTEMP_COLD_LVL_BIT    = NO_OS_BIT(12),
	LTC3350_DTEMP_HOT_LVL_BIT     = NO_OS_BIT(13),
	LTC3350_ESR_HI_LVL_BIT        = NO_OS_BIT(14),
	LTC3350_CAP_LO_LVL_BIT        = NO_OS_BIT(15)
};

/**
 * @brief Structure holding the parameters for LTC3350 device initialization.
 * @struct ltc3350_init_param
 */
struct ltc3350_init_param {
	/** Device Communication initialization structure I2C */
	struct no_os_i2c_init_param i2c_init;
};

/**
 * @brief LTC3350 Device structure.
 * @struct ltc3350_dev
 */
struct ltc3350_dev {
	/** Device I2C Communication */
	struct no_os_i2c_desc *i2c_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! NO OS Requirements */
/*! Initialize the device driver before use. */
int ltc3350_init(struct ltc3350_dev **device,
		 struct ltc3350_init_param *init_param);
/*! Free memory allocated by ltc3350_init(). */
int ltc3350_remove(struct ltc3350_dev *dev);

/*! General functions */
/*! Read a device register through the I2C interface. */
int ltc3350_read_device_data(struct ltc3350_dev *dev, uint8_t base_address,
			     uint16_t *read_data);
/*! Write a device register through the I2C interface. */
int ltc3350_write_device_data(struct ltc3350_dev *dev, uint8_t base_address,
			      uint16_t write_data);

/*! LTC3350 functions */

/* LTC3350_USE_MEASUREMENTS == 1 */

#if LTC3350_USE_MEASUREMENTS

/*! Gets number of capacitors configured */
int ltc3350_get_num_caps(struct ltc3350_dev *dev, uint8_t *num_caps);

/*! Reads the ESR data. */
int ltc3350_get_esr(struct ltc3350_dev *dev, uint16_t *value);
/*! Reads the Vcap data for selected n_cap, 0 for generic. */
int ltc3350_get_vcap(struct ltc3350_dev *dev, uint8_t n_cap, uint16_t *value);

#endif

/* LTC3350_USE_ALARMS == 0 */

#if LTC3350_USE_ALARMS

/*! Set an specific alarm and its related threshold value. */
int ltc3350_set_alarm(struct ltc3350_dev *dev, enum ltc3350_enum_alarms alarm,
		      uint16_t alarm_threshold);
/*! Mute an specific alarm. */
int ltc3350_mute_alarm(struct ltc3350_dev *dev, enum ltc3350_enum_alarms alarm);
/*! Get alarms status mask*/
int ltc3350_get_alarm_status_mask(struct ltc3350_dev *dev,
				  uint16_t *alarm_mask);
/*! Clear alarms status mask*/
int ltc3350_clear_alarm_status_mask(struct ltc3350_dev *dev);

#endif

#endif /* __LTC3350_H__ */
