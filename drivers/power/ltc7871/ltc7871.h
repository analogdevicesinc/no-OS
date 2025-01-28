/***************************************************************************//**
 *   @file   LTC7871.h
 *   @brief  Header file for the LTC7871 Driver
 *   @author Aldrin Abacan (aldrin.abacan@analog.com)
 *******************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef __LTC7871_H__
#define __LTC7871_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_units.h"
#include "no_os_util.h"

#define SET 1
#define CLEAR 0

#define LTC7871_CRC_POLYNOMIAL          0x07

#define LTC7871_FRAME_SIZE       		3
#define LTC7871_NUM_REGISTERS           8

/* Miscellaneous Definitions*/
#define LTC7871_SPI_READ			0x01
#define LTC7871_SPI_WRITE			0x00

// LTC7871 SPI Command
#define LTC7871_STATUS_ACQUISITION_COMMAND    0xF0
#define LTC7871_DATA_WRITE_COMMAND            0xF2
#define LTC7871_DATA_READ_COMMAND             0xF4

// LTC7871 SPI Command Size in Bytes
#define LTC7871_STATUS_ACQUISITION_COMMAND_SIZE   4
#define LTC7871_DATA_WRITE_COMMAND_SIZE           8
#define LTC7871_DATA_READ_COMMAND_SIZE            8

// LTC7871 SPI Register Address
#define LTC7871_MFR_FAULT                   0x01
#define LTC7871_MFR_OC_FAULT                0x02
#define LTC7871_MFR_NOC_FAULT               0x03
#define LTC7871_MFR_STATUS                  0x04
#define LTC7871_MFR_CONFIG1                 0x05
#define LTC7871_MFR_CONFIG2                 0x06
#define LTC7871_CHIP_CTRL                   0x07
#define LTC7871_IDAC_VLOW                   0x08
#define LTC7871_IDAC_VHIGH                  0x09
#define LTC7871_IDAC_SETCUR                 0x0A
#define LTC7871_MFR_SSFM                    0x0B

// LTC7871 SPI Register Mask
#define LTC7871_MFR_FAULT_MASK         		  NO_OS_GENMASK(6, 0)
#define LTC7871_MFR_OC_FAULT_MASK             NO_OS_GENMASK(6, 0)
#define LTC7871_MFR_NOC_FAULT_MASK            NO_OS_GENMASK(6, 0)
#define LTC7871_MFR_STATUS_MASK               NO_OS_GENMASK(2, 0)
#define LTC7871_MFR_CONFIG1_MASK              NO_OS_GENMASK(5, 0)
#define LTC7871_MFR_CONFIG2_MASK              NO_OS_GENMASK(4, 0)
#define LTC7871_MFR_CHIP_CTRL_MASK            NO_OS_GENMASK(2, 0)
#define LTC7871_MFR_SSFM_MASK           	  NO_OS_GENMASK(8, 0)


// LTC7871 MFR_FAULT register bits
#define LTC7871_VLOW_OV_MASK                  NO_OS_BIT(6)
#define LTC7871_VHIGH_OV_MASK                 NO_OS_BIT(5)
#define LTC7871_VHIGH_UV_MASK                 NO_OS_BIT(4)
#define LTC7871_DRVCC_UV_MASK                 NO_OS_BIT(3)
#define LTC7871_V5_UV_MASK                    NO_OS_BIT(2)
#define LTC7871_VREF_BAD_MASK                 NO_OS_BIT(1)
#define LTC7871_OVER_TEMP_MASK                NO_OS_BIT(0)

// LTC7871 MFR_OC_FAULT register bits
#define LTC7871_OC_FAULT_6_MASK               NO_OS_BIT(5)
#define LTC7871_OC_FAULT_5_MASK               NO_OS_BIT(4)
#define LTC7871_OC_FAULT_4_MASK               NO_OS_BIT(3)
#define LTC7871_OC_FAULT_3_MASK               NO_OS_BIT(2)
#define LTC7871_OC_FAULT_2_MASK               NO_OS_BIT(1)
#define LTC7871_OC_FAULT_1_MASK               NO_OS_BIT(0)

// LTC7871 MFR_NOC_FAULT register bits
#define LTC7871_NOC_FAULT_6_MASK               NO_OS_BIT(5)
#define LTC7871_NOC_FAULT_5_MASK               NO_OS_BIT(4)
#define LTC7871_NOC_FAULT_4_MASK               NO_OS_BIT(3)
#define LTC7871_NOC_FAULT_3_MASK               NO_OS_BIT(2)
#define LTC7871_NOC_FAULT_2_MASK               NO_OS_BIT(1)
#define LTC7871_NOC_FAULT_1_MASK               NO_OS_BIT(0)

// LTC7871 MFR_STATUS register bits
#define LTC7871_SS_DONE_MASK                   NO_OS_BIT(2)
#define LTC7871_MAX_CURRENT_MASK               NO_OS_BIT(1)
#define LTC7871_PGOOD_MASK                     NO_OS_BIT(0)

// LTC7871 MFR_CONFIG1 register bits
#define LTC7871_SERCUR_WARNING_MASK            NO_OS_BIT(5)
#define LTC7871_DRVCC_SET_MASK                 NO_OS_GENMASK(4, 3)
#define LTC7871_ILIM_SET_MASK                  NO_OS_GENMASK(2, 0)

// LTC7871 MFR_CONFIG2 register bits
#define LTC7871_BURST_MASK                     NO_OS_BIT(4)
#define LTC7871_DCM_MASK                       NO_OS_BIT(3)
#define LTC7871_HIZ_MASK                       NO_OS_BIT(2)
#define LTC7871_SPRD_MASK                      NO_OS_BIT(1)
#define LTC7871_BUCK_BOOST_MASK                NO_OS_BIT(0)

// LTC7871 MFR_CHIP_CTRL register bits
#define LTC7871_CML_MASK                       NO_OS_BIT(2)
#define LTC7871_RESET_MASK                     NO_OS_BIT(1)
#define LTC7871_WP_MASK                        NO_OS_BIT(0)

// LTC7871 MFR_IDAC_VLOW register bits
#define LTC7871_MFR_IDAC_VLOW_MASK             NO_OS_GENMASK(6, 0)

// LTC7871 MFR_IDAC_VHIGH register bits
#define LTC7871_MFR_IDAC_VHIGH_MASK            NO_OS_GENMASK(6, 0)

// LTC7871 MFR_IDAC_SETCUR register bits
#define LTC7871_MFR_IDAC_SETCUR_MASK           NO_OS_GENMASK(4, 0)

// LTC7871 MFR_SSFM register bits
#define LTC7871_MFR_SSFM_FSR_MASK              NO_OS_GENMASK(4, 3)
#define LTC7871_MFR_SSFM_MSF_MASK              NO_OS_GENMASK(2, 0)


/**
 * @enum ltc7871_ssfm_fsr
 * @brief Frequency Spread Range control bits
 */
enum ltc7871_ssfm_fsr {
	/** +/-12%*/
	LTC7871_SSFM_FSR_12,
	/** +/-15%*/
	LTC7871_SSFM_FSR_15,
	/** +/-10%*/
	LTC7871_SSFM_FSR_10,
	/** +/-8%*/
	LTC7871_SSFM_FSR_8
};

/**
 * @enum ltc7871_ssfm_msf
 * @brief Modulation Signal Frequency control bits
 */
enum ltc7871_ssfm_msf {
	/** Controller Switching Frequency/512*/
	LTC7871_SSFM_MSF_512,
	/** Controller Switching Frequency/1024*/
	LTC7871_SSFM_MSF_1024,
	/** Controller Switching Frequency/2048*/
	LTC7871_SSFM_MSF_2048,
	/** Controller Switching Frequency/4096*/
	LTC7871_SSFM_MSF_4096,
	/** Controller Switching Frequency/256*/
	LTC7871_SSFM_MSF_256,
	/** Controller Switching Frequency/128*/
	LTC7871_SSFM_MSF_128,
	/** Controller Switching Frequency/64*/
	LTC7871_SSFM_MSF_64,
	/** Controller Switching Frequency/512*/
	LTC7871_SSFM_MSF_512U
};

/**
 * @enum ltc7871_ctrl_wp
 * @brief Frequency Spread Range control bits
 */
enum ltc7871_ctrl_wp {
	/** enable write protect*/
	LTC7871_CTRL_WP_ENABLE,
	/** disable write protect*/
	LTC7871_CTRL_WP_DISABLE
};

/**
 * @struct ltc7871_init_param
 * @brief Initialization parameter for the LTC7871 device.
 */
struct ltc7871_init_param {
	struct no_os_spi_init_param *spi;
	struct no_os_gpio_init_param *gpio_pwmen;
};

/**
 * @struct ltc7871_dev
 * @brief Device descriptor for ltc7871.
 */
struct ltc7871_dev {
	struct no_os_spi_desc *spi;
	struct no_os_gpio_desc *gpio_pwmen;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** Register Read */
uint8_t ltc7871_get_pec_byte(uint8_t *data, uint8_t len);

/** Register Read */
int ltc7871_reg_read(struct ltc7871_dev *dev, uint8_t reg, uint8_t *data);

/** Register Write */
int ltc7871_reg_write(struct ltc7871_dev *dev, uint8_t reg, uint8_t data);

/** Register mask Write */
int ltc7871_reg_write_mask(struct ltc7871_dev *dev, uint8_t address,
			   uint8_t mask, uint8_t data);

/**Read Most critical fault status */
int ltc7871_get_mfr_fault(struct ltc7871_dev *dev, uint8_t status, bool *value);

/**Read overcurrent condition status */
int ltc7871_get_mfr_oc_fault(struct ltc7871_dev *dev, uint8_t status,
			     bool *value);

/**Read negative overcurrent condition status */
int ltc7871_get_mfr_noc_fault(struct ltc7871_dev *dev, uint8_t status,
			      bool *value);

/**gets configuration of the controller programmed by the pins.*/
int ltc7871_get_mfr_config1_setting(struct ltc7871_dev *dev,
				    uint8_t config, uint8_t *value);

/**gets configuration of the controller programmed by the pins.*/
int ltc7871_get_mfr_config2_setting(struct ltc7871_dev *dev,
				    uint8_t config, uint8_t *value);

/**gets status */
int ltc7871_get_mfr_status(struct ltc7871_dev *dev, uint8_t status_mask,
			   bool *status);

/**Reset all R/W registers */
int ltc7871_reset(struct ltc7871_dev *dev);

/**Clear PEC fault bit */
int ltc7871_clear_pec_fault(struct ltc7871_dev *dev);

/**Read PEC fault bit */
int ltc7871_read_pec_fault(struct ltc7871_dev *dev, bool *value);

/**et write protect bit */
int ltc7871_set_write_protect(struct ltc7871_dev *dev, bool value);

/**Read Write Protect bit for IDAC registers, and MFR_SSFM register */
int ltc7871_is_write_protected(struct ltc7871_dev *dev, bool *value);

/**get the current DAC value to program the VLOW voltage.*/
int ltc7871_get_mfr_idac_vlow(struct ltc7871_dev *dev, int8_t *value);

/**stores the current DAC value to program the VLOW voltage.  */
int ltc7871_set_mfr_idac_vlow(struct ltc7871_dev *dev, int8_t value);

/**get the current DAC value to program the VHIGH voltage.*/
int ltc7871_get_mfr_idac_vhigh(struct ltc7871_dev *dev, int8_t *value);

/**stores the current DAC value to program the VHIGH voltage. */
int ltc7871_set_mfr_idac_vhigh(struct ltc7871_dev *dev, int8_t value);

/**get the current DAC value to program the sourcing current of the SETCUR pin.*/
int ltc7871_get_mfr_idac_setcur(struct ltc7871_dev *dev, int8_t *value);

/**stores the current DAC value to program the sourcing current of the SETCUR pin. */
int ltc7871_set_mfr_idac_setcur(struct ltc7871_dev *dev, int8_t value);

/**Get Frequency Spread Range.*/
int ltc7871_get_freq_spread_range(struct ltc7871_dev *dev, uint8_t *value);

/**Set Frequency Spread Range */
int ltc7871_set_freq_spread_range(struct ltc7871_dev *dev,
				  enum ltc7871_ssfm_fsr value);

/**Get Modulation Signal Frequency */
int ltc7871_get_mod_freq(struct ltc7871_dev *dev, uint8_t *value);

/**Set Modulation Signal Frequency */
int ltc7871_set_mod_freq(struct ltc7871_dev *dev, enum ltc7871_ssfm_msf value);

/** Set PWMEN pin of LTC7871 device. */
int ltc7871_set_pwmen_pin(struct ltc7871_dev *dev, uint8_t value);

/** Get PWMEN pin */
int ltc7871_get_pwmen_pin(struct ltc7871_dev *dev, uint8_t *value);

/** Initialize the LTC7871 device descriptor. */
int ltc7871_init(struct ltc7871_dev **device,
		 struct ltc7871_init_param *init_param);

/** Remove resources allocated by the init function. */
int ltc7871_remove(struct ltc7871_dev *dev);

#endif /** __LTC7871_H__ */
