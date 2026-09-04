/*******************************************************************************
 * @file    max20362.h
 * @brief   Header file for MAX20362 Micro-Battery PMIC driver
 * @author  Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __MAX20362_H__
#define __MAX20362_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_error.h"

/** I2C slave address */
#define MAX20362_PMIC_I2C_ADDR			0x68

/** Expected Chip ID (Vendor: 0x0, Revision: 0x0) */
#define MAX20362_CHIP_ID_VAL			0x00

/** Register Map */
#define MAX20362_REG_CHIP_ID			0x00  /**< Chip ID register */
#define MAX20362_REG_BBST_CFG0			0x01  /**< Buck-boost configuration 0 */
#define MAX20362_REG_BBST_VSET			0x02  /**< Buck-boost voltage setting */
#define MAX20362_REG_CAP_VSET			0x0A  /**< Capacitor voltage setting */
#define MAX20362_REG_IGN_CFG			0x0C  /**< Input/Ingenuity config */
#define MAX20362_REG_STATUS			0x10  /**< Status register */
#define MAX20362_REG_INT			0x14  /**< Main interrupt register */
#define MAX20362_REG_INGEN_INT			0x16  /**< Ingenuity interrupt register */
#define MAX20362_REG_LDO_INT			0x17  /**< LDO interrupt register */
#define MAX20362_REG_INT_MASK			0x18  /**< Main interrupt mask */
#define MAX20362_REG_INGEN_INT_MASK		0x1A  /**< Ingenuity interrupt mask */
#define MAX20362_REG_LDO_INT_MASK		0x1B  /**< LDO interrupt mask */
#define MAX20362_REG_RR_VSET_BASE		0x20  /**< Round-robin voltage table base (0x20-0x33) */
#define MAX20362_REG_LDO_CFG			0x40  /**< LDO configuration */
#define MAX20362_REG_LDO_VSET			0x41  /**< LDO voltage setting */
#define MAX20362_REG_LOCK_MSK			0x50  /**< Lock mask register */
#define MAX20362_REG_LOCK_UNLOCK		0x51  /**< Lock/unlock password */
#define MAX20362_REG_DVS_CFG			0x54  /**< DVS configuration */
#define MAX20362_REG_RR_CFG1			0x55  /**< Round-robin configuration 1 (Size) */

/** Lock/Unlock Passwords */
#define MAX20362_UNLOCK_PASSWORD		0x55  /**< Unlock write protection */
#define MAX20362_LOCK_PASSWORD			0xAA  /**< Lock write protection */

/** Lock Mask Definitions */
#define MAX20362_LOCK_MASK_UNLOCKED  	0x00  /**< BBLck bit = 0 (unlocked) */
#define MAX20362_LOCK_MASK_LOCKED    	0x01  /**< BBLck bit = 1 (locked) */

/** CHIP_ID Register Bits */
#define MAX20362_CHIPID_VENDOR_MSK		NO_OS_GENMASK(7, 4)
#define MAX20362_CHIPID_REV_MSK			NO_OS_GENMASK(3, 0)

/** BBstCfg0 Register Bits (0x01) */
#define MAX20362_BBSTCFG_BBHALFBW_MSK		NO_OS_BIT(7)     /**< Half bandwidth mode */
#define MAX20362_BBSTCFG_BBSTLOWEMI_MSK		NO_OS_BIT(6)     /**< Low EMI mode */
#define MAX20362_BBSTCFG_BBSTENA_MSK		NO_OS_BIT(5)     /**< Buck-boost enable */
#define MAX20362_BBSTCFG_BBSTFAST_MSK		NO_OS_BIT(4)     /**< Fast FPWM mode */
#define MAX20362_BBSTCFG_BBFHIGHSH_MSK		NO_OS_BIT(2)     /**< High side FET control */
#define MAX20362_BBSTCFG_BBSTACTDSC_MSK		NO_OS_BIT(1)     /**< Active discharge */
#define MAX20362_BBSTCFG_BBSTPSVDSC_MSK		NO_OS_BIT(0)     /**< Passive discharge */

/** BBST_VSET Register Bits */
#define MAX20362_BBSTVSET_MSK			NO_OS_GENMASK(6, 0)

/** CAP_VSET Register Bits */
#define MAX20362_CAPVSET_ACTDSC_MSK		NO_OS_BIT(7)     	/**< Active discharge */
#define MAX20362_CAPVSET_PSVDSC_MSK		NO_OS_BIT(6)     	/**< Passive discharge */
#define MAX20362_CAPVSET_STEP_MSK		NO_OS_GENMASK(5, 4) /**< Voltage step size */
#define MAX20362_CAPVSET_VSET_MSK		NO_OS_GENMASK(3, 0) /**< Voltage setting */

/** IGN_CFG Register Bits */
#define MAX20362_IGNCFG_BBVDROP_MSK		NO_OS_GENMASK(7, 6) /**< Battery droop */
#define MAX20362_IGNCFG_ILIM_MSK		NO_OS_GENMASK(5, 0) /**< Current limit */

/** LDO_CFG Register Bits */
#define MAX20362_LDOCFG_BBSTSUP_MSK		NO_OS_BIT(5)     /**< LDO source select */
#define MAX20362_LDOCFG_PROPMD_MSK		NO_OS_BIT(4)     /**< Proportional mode */
#define MAX20362_LDOCFG_LOWIQ_MSK		NO_OS_BIT(3)     /**< Low IQ mode */
#define MAX20362_LDOCFG_ACT_MSK			NO_OS_BIT(2)     /**< Active discharge */
#define MAX20362_LDOCFG_PSV_MSK			NO_OS_BIT(1)     /**< Passive discharge */
#define MAX20362_LDOCFG_ENA_MSK			NO_OS_BIT(0)     /**< LDO enable */

/** LDO_VSET Register Bits */
#define MAX20362_LDOVSET_MSK			NO_OS_GENMASK(4, 0)

/** DVS_CFG Register Bits */
#define MAX20362_DVSCFG_I2CLKPTDIS_MSK  NO_OS_BIT(3)     	/**< I2C lockup timer disable */
#define MAX20362_DVSCFG_RRWRAP_MSK      NO_OS_BIT(2)     	/**< RR wrap enable */
#define MAX20362_DVSCFG_RRENA_MSK       NO_OS_BIT(1)     	/**< Round-robin enable */
#define MAX20362_DVSCFG_DVSSOURCE_MSK   NO_OS_BIT(0) 		/**< DVS source */

/** RRCfg1 Register Bits (0x55) */
#define MAX20362_RRCFG1_RRSIZE_MSK      NO_OS_GENMASK(4, 0) /**< RR table size */

/** STATUS Register Bits */
#define MAX20362_STATUS_CAPOVLO_MSK		NO_OS_BIT(7)     /**< Cap overvoltage */
#define MAX20362_STATUS_CAPUVLO_MSK		NO_OS_BIT(6)     /**< Cap undervoltage */
#define MAX20362_STATUS_BBSTON_MSK		NO_OS_BIT(5)     /**< Buck-boost on */
#define MAX20362_STATUS_BBSTOFF_MSK		NO_OS_BIT(4)     /**< Buck-boost off */
#define MAX20362_STATUS_BBINUVLO_MSK		NO_OS_BIT(3)     /**< BB input UVLO */
#define MAX20362_STATUS_UVLO_MSK		NO_OS_BIT(2)     /**< Undervoltage lockout */
#define MAX20362_STATUS_BSTFLT_MSK		NO_OS_BIT(1)     /**< Boost fault */
#define MAX20362_STATUS_THMFLT_MSK		NO_OS_BIT(0)     /**< Thermal fault */

/** INT and INT_MASK Register Bits */
#define MAX20362_INT_CAPOVLO_MSK		NO_OS_BIT(7)     /**< Cap overvoltage */
#define MAX20362_INT_CAPUVLO_MSK		NO_OS_BIT(6)     /**< Cap undervoltage */
#define MAX20362_INT_BBSTON_MSK			NO_OS_BIT(5)     /**< Buck-boost on */
#define MAX20362_INT_BBSTOFF_MSK		NO_OS_BIT(4)     /**< Buck-boost off */
#define MAX20362_INT_BBINUVLO_MSK		NO_OS_BIT(3)     /**< BB input UVLO */
#define MAX20362_INT_UVLO_MSK			NO_OS_BIT(2)     /**< UVLO */
#define MAX20362_INT_BSTFLT_MSK			NO_OS_BIT(1)     /**< Boost fault */
#define MAX20362_INT_THMFLT_MSK			NO_OS_BIT(0)     /**< Thermal fault */

/** LDO_INT and LDO_INT_MASK Register Bits */
#define MAX20362_LDOINT_DIV_MSK			NO_OS_BIT(3)     /**< Divider fault */
#define MAX20362_LDOINT_SHR_MSK			NO_OS_BIT(2)     /**< Short circuit */
#define MAX20362_LDOINT_THM_MSK			NO_OS_BIT(1)     /**< Thermal fault */
#define MAX20362_LDOINT_CLP_MSK			NO_OS_BIT(0)     /**< Current limit fault */

/** INGEN_INT and INGEN_INT_MASK Register Bits */
#define MAX20362_INGENINT_OUTTMO_MSK	NO_OS_BIT(4)     /**< Output timeout */
#define MAX20362_INGENINT_DRPMIN_MSK	NO_OS_BIT(3)     /**< Droop min */
#define MAX20362_INGENINT_TNKTMO_MSK	NO_OS_BIT(2)     /**< Tank timeout */
#define MAX20362_INGENINT_SIMOPIN_MSK	NO_OS_BIT(1)     /**< SIMO pin */
#define MAX20362_INGENINT_DRPMAX_MSK	NO_OS_BIT(0)     /**< Droop max */

/** Interrupt Mask Values (0=unmask/enable, 1=mask/disable) */
#define MAX20362_INT_EN				0xCF     /**< Enable main interrupts */
#define MAX20362_LDO_INT_EN			0x0F     /**< Enable LDO interrupts */
#define MAX20362_INGEN_INT_EN			0x1D     /**< Enable Ingenuity interrupts */

/** Interrupt Mask Control Values */
#define MAX20362_INT_UNMASK_ALL      		0x00  /**< Unmask all interrupts */
#define MAX20362_INT_MASK_ALL        		0xFF  /**< Mask all interrupts */

/** Voltage Range Constants (microvolts) */
#define MAX20362_BBOUT_MIN_UV			1500000  /**< Buck-boost min: 1.5V */
#define MAX20362_BBOUT_MAX_UV			5500000  /**< Buck-boost max: 5.5V */
#define MAX20362_BBOUT_STEP_UV			50000    /**< Buck-boost step: 50mV */
#define MAX20362_LDO_MIN_UV			900000   /**< LDO min: 0.9V */
#define MAX20362_LDO_MAX_UV			4000000  /**< LDO max: 4.0V */
#define MAX20362_LDO_STEP_UV			100000   /**< LDO step: 100mV */
#define MAX20362_VCAP_MIN_UV			1600000  /**< Cap min: 1.6V */
#define MAX20362_VCAP_MAX_UV			9500000  /**< Cap max: 9.5V */
#define MAX20362_VCAP_STEP_UV			250000   /**< Cap step: 250mV */

/** Capacitor Voltage Step Sizes */
#define MAX20362_VCAP_STEP_500MV		0
#define MAX20362_VCAP_STEP_250MV		1
#define MAX20362_VCAP_STEP_125MV		2

/** Capacitor Voltage Ranges (microvolts) */
#define MAX20362_VCAP_500MV_MIN_UV		2500000
#define MAX20362_VCAP_500MV_MAX_UV		9500000
#define MAX20362_VCAP_500MV_STEP_UV		500000
#define MAX20362_VCAP_250MV_MIN_UV		1600000
#define MAX20362_VCAP_250MV_MAX_UV		5350000
#define MAX20362_VCAP_250MV_STEP_UV		250000
#define MAX20362_VCAP_125MV_MIN_UV		1650000
#define MAX20362_VCAP_125MV_MAX_UV		3025000
#define MAX20362_VCAP_125MV_STEP_UV		125000

/** Capacitor Voltage Setting Maximum Values */
#define MAX20362_VCAP_500MV_VSET_MAX		14       	/**< Max VSET value for 500mV steps */
#define MAX20362_VCAP_250MV_VSET_MAX		15       	/**< Max VSET value for 250mV steps */
#define MAX20362_VCAP_125MV_VSET_MAX		15       	/**< Max VSET value for 125mV steps */
#define MAX20362_VCAP_125MV_BASE_OFFSET		4        	/**< Base offset for 125mV step calculations */
#define MAX20362_VCAP_125MV_UPPER_LIMIT_UV	2900000  	/**< Upper limit for 125mV step calculation */

/** Current Limit Constants (milliamps) */
#define MAX20362_ILIM_MIN_MA			5        /**< Min current limit: 5mA */
#define MAX20362_ILIM_MAX_MA			50       /**< Max current limit: 50mA */
#define MAX20362_ILIM_STEP_MA			1        /**< Current limit step: 1mA */

/** Timing Constants (microseconds) */
#define MAX20362_DVS_SETTLE_DELAY_US		300      /**< DVS mode change delay: 300µs */
#define MAX20362_VOLTAGE_SETTLE_MS		10       /**< General voltage settle delay: 10ms */

/** Round-Robin Constants */
#define MAX20362_RR_MAX_VOLTAGES		20       					/**< Max RR table entries */
#define MAX20362_RR_REGISTER_START		0x20     					/**< First RR register address */
#define MAX20362_RR_MIN_COUNT			1        					/**< Minimum RR table count */
#define MAX20362_RR_VSET_ADDR(x) (MAX20362_REG_RR_VSET_BASE + (x))	/**< Index RR table */

/** Register Value Constants */
#define MAX20362_REGISTER_COUNT_OFFSET		1        /**< Offset for count-based register values */

/** DVS Mode Values */
#define MAX20362_DVS_I2C_MODE			0x00     /**< DVS I2C control mode value */
#define MAX20362_DVS_PSPI_MODE			0x01     /**< DVS pSPI control mode value */
#define MAX20362_DVS_AUTO_MODE			0x02     /**< DVS round-robin mode value */

/** Bit Masks and Field Values */
#define MAX20362_VOLTAGE_MASK_7BIT		0x7F     /**< 7-bit voltage register mask */
#define MAX20362_DVS_VSET_MASK			0x0F     /**< DVS VSET field mask */
#define MAX20362_INTERRUPT_CLEAR_ALL		0xFF     /**< Clear all interrupts value */

/**
 * @enum max20362_dvs_source
 * @brief Dynamic voltage scaling source selection
 */
enum max20362_dvs_source {
	MAX20362_DVS_SOURCE_I2C,         /**< Control via I2C register */
	MAX20362_DVS_SOURCE_PSPI,        /**< Control via Pseudo-SPI pins */
	MAX20362_DVS_SOURCE_ROUND_ROBIN  /**< Cycle through RR table */
};

/**
 * @enum max20362_bbat_vdrop
 * @brief Battery voltage droop configuration
 */
enum max20362_bbat_vdrop {
	MAX20362_BBAT_VDROP_55MV,   /**< 55mV droop */
	MAX20362_BBAT_VDROP_100MV,  /**< 100mV droop */
	MAX20362_BBAT_VDROP_150MV,  /**< 150mV droop */
	MAX20362_BBAT_VDROP_200MV   /**< 200mV droop */
};

/**
 * @enum max20362_ldo_source
 * @brief LDO input source selection
 * @details This is an abstraction. The hardware selection is controlled by the
 * LDOBBstSupp bit in the LDOCfg register (0x40), which toggles
 * between the battery input and the buck-boost output. The driver
 * must manage this selection logic.
 */
enum max20362_ldo_source {
	MAX20362_LDO_SOURCE_BBOUT,  /**< Buck-boost output */
	MAX20362_LDO_SOURCE_CAP,    /**< Storage capacitor */
	MAX20362_LDO_SOURCE_BATT    /**< Battery input */
};

/**
 * @struct max20362_dev
 * @brief Device descriptor for MAX20362
 */
struct max20362_dev {
	struct no_os_i2c_desc *i2c_desc;  		/**< I2C descriptor */
	enum max20362_ldo_source ldo_source;	/**< LDO input source */
};

/**
 * @struct max20362_init_param
 * @brief Initialization parameters for MAX20362
 */
struct max20362_init_param {
	struct no_os_i2c_init_param i2c_init;         /**< I2C init params */
	uint32_t buck_boost_voltage_uv;               /**< Buck-boost voltage (uV) */
	uint32_t cap_voltage_uv;                      /**< Capacitor voltage (uV) */
	uint32_t ldo_voltage_uv;                      /**< LDO voltage (uV) */
	enum max20362_bbat_vdrop bbat_vdrop;          /**< Battery voltage droop */
	uint8_t input_current_limit_ma;               /**< Input current limit (mA) */
	bool buck_boost_enable;                       /**< Enable buck-boost */
	bool ldo_enable;                              /**< Enable LDO */
};

/* Device Management */

/* Initialize MAX20362 device */
int max20362_init(struct max20362_dev **device,
		  const struct max20362_init_param *init_param);

/* Free resources and remove device */
int max20362_remove(struct max20362_dev *dev);

/* Register Access */

/* Write register value */
int max20362_reg_write(struct max20362_dev *dev, uint8_t reg, uint8_t val);

/* Read register value */
int max20362_reg_read(struct max20362_dev *dev, uint8_t reg, uint8_t *val);

/* Update register bits with mask */
int max20362_reg_update_bits(struct max20362_dev *dev, uint8_t reg,
			     uint8_t mask, uint8_t val);

/* Buck-Boost Control */

/* Enable or disable buck-boost converter */
int max20362_enable_buck_boost(struct max20362_dev *dev, bool enable);

/* Set buck-boost output voltage (1.5V-5.5V) */
int max20362_set_buck_boost_voltage(struct max20362_dev *dev,
				    uint32_t microvolts);

/* Configure buck-boost operating mode */
int max20362_config_buck_boost_mode(struct max20362_dev *dev,
				    bool half_bw, bool low_emi, bool fast_fpwm);

/* Configure buck-boost discharge modes */
int max20362_config_buck_boost_discharge(struct max20362_dev *dev,
		bool active_discharge, bool passive_discharge);

/* LDO Control */

/* Enable or disable LDO regulator */
int max20362_enable_ldo(struct max20362_dev *dev, bool enable);

/* Set LDO output voltage (0.9V-4.0V) */
int max20362_set_ldo_voltage(struct max20362_dev *dev, uint32_t microvolts);

/* Configure LDO operating modes */
int max20362_config_ldo_mode(struct max20362_dev *dev, bool low_iq_mode,
			     bool passive_discharge, bool active_discharge);

/* Set LDO input source selection */
int max20362_set_ldo_input_source(struct max20362_dev *dev,
				  enum max20362_ldo_source source);

/* DVS Control */

/* Set DVS operating mode */
int max20362_set_dvs_mode(struct max20362_dev *dev,
			  enum max20362_dvs_source source);

/* Configure DVS round-robin voltage table */
int max20362_set_dvs_rr_table(struct max20362_dev *dev,
			      const uint32_t *voltages_uv, uint8_t count);

/* Capacitor and Current Control */

/* Set energy storage capacitor voltage */
int max20362_set_cap_voltage(struct max20362_dev *dev, uint32_t microvolts,
			     uint8_t step_size);

/* Set input current limit (5-50mA) */
int max20362_set_input_current_limit(struct max20362_dev *dev, uint8_t ma);

/* Set battery voltage droop configuration */
int max20362_set_bbat_vdrop(struct max20362_dev *dev,
			    enum max20362_bbat_vdrop vdrop);

/* Status and Interrupts */

/* Read device fault status flags */
int max20362_get_status(struct max20362_dev *dev, uint8_t *status);

/* Read main interrupt status register */
int max20362_get_int_status(struct max20362_dev *dev, uint8_t *status);

/* Read LDO interrupt status register */
int max20362_get_ldo_int_status(struct max20362_dev *dev, uint8_t *status);

/* Read Ingenuity interrupt status register */
int max20362_get_ingen_int_status(struct max20362_dev *dev, uint8_t *status);

/* Set main interrupt mask register */
int max20362_set_int_mask(struct max20362_dev *dev, uint8_t mask);

/* Set LDO interrupt mask register */
int max20362_set_ldo_int_mask(struct max20362_dev *dev, uint8_t mask);

/* Set Ingenuity interrupt mask register */
int max20362_set_ingen_int_mask(struct max20362_dev *dev, uint8_t mask);

/* Clear all interrupt status flags */
int max20362_clear_all_interrupts(struct max20362_dev *dev);

/* Lock/Unlock Control */

/* Unlock write protection for registers */
int max20362_unlock_registers(struct max20362_dev *dev);

/* Lock write protection for registers */
int max20362_lock_registers(struct max20362_dev *dev);

#endif // __MAX20362_H__
