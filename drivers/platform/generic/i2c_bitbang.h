/***************************************************************************//**
 *   @file   i2c_bitbang.h
 *   @brief  Header file for I2C bitbang implementation
 *   @author Generated with Claude Code
*******************************************************************************/

#ifndef I2C_BITBANG_H_
#define I2C_BITBANG_H_

/******************************************************************************/
/***************************** Include Files *********************************/
/******************************************************************************/

#include "no_os_i2c.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions ********************/
/******************************************************************************/

/* I2C standard speeds */
#define I2C_BITBANG_SPEED_STANDARD	100000  /* 100 kHz */
#define I2C_BITBANG_SPEED_FAST		400000  /* 400 kHz */

/* Timeout for clock stretching (microseconds) */
#define I2C_BITBANG_TIMEOUT_US		100000  /* 100 ms */

/******************************************************************************/
/*************************** Types Declarations ******************************/
/******************************************************************************/

/**
 * @enum i2c_bitbang_pull_type
 * @brief Pull-up configuration options
 */
enum i2c_bitbang_pull_type {
	/** External pull-ups present, use open-drain */
	I2C_BITBANG_PULL_EXTERNAL,
	/** Use internal pull-ups */
	I2C_BITBANG_PULL_INTERNAL,
	/** No pull-ups, drive high/low actively */
	I2C_BITBANG_PULL_NONE
};

/**
 * @struct i2c_bitbang_init_param
 * @brief I2C bitbang initialization parameters
 */
struct i2c_bitbang_init_param {
	/** GPIO initialization parameters for SDA */
	struct no_os_gpio_init_param sda_init;
	/** GPIO initialization parameters for SCL */
	struct no_os_gpio_init_param scl_init;
	/** Pull-up configuration */
	enum i2c_bitbang_pull_type pull_type;
	/** Clock stretching timeout in microseconds */
	uint32_t timeout_us;
};

/**
 * @struct i2c_bitbang_desc
 * @brief I2C bitbang descriptor
 */
struct i2c_bitbang_desc {
	/** SDA GPIO descriptor */
	struct no_os_gpio_desc *sda;
	/** SCL GPIO descriptor */
	struct no_os_gpio_desc *scl;
	/** Half period delay in microseconds */
	uint32_t half_period_us;
	/** Quarter period delay in microseconds */
	uint32_t quarter_period_us;
	/** Pull-up configuration */
	enum i2c_bitbang_pull_type pull_type;
	/** Clock stretching timeout */
	uint32_t timeout_us;
};

/******************************************************************************/
/************************ Functions Declarations ****************************/
/******************************************************************************/

/* I2C bitbang platform operations */
extern const struct no_os_i2c_platform_ops i2c_bitbang_ops;

#endif /* I2C_BITBANG_H_ */