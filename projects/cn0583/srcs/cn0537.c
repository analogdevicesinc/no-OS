/***************************************************************************//**
*   @file   cn0537.c
*   @brief  CN0537 application source.
*
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright (c) 2020 Analog Devices, Inc. All Rights Reserved.
* This software is proprietary to Analog Devices, Inc. and its licensors.
*
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "cn0537.h"
#include "no_os_error.h"
#include "maxim_spi.h"
#include "maxim_uart.h"
#include "maxim_rtc.h"
#include "no_os_delay.h"
#include "maxim_irq.h"
#include "maxim_gpio_irq.h"
#include "maxim_uart_stdio.h"

#include "no_os_uart.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_rtc.h"

#include "no_os_irq.h"
#include "maxim_gpio.h"
#include "max31875.h"
#include "maxim_i2c.h"


/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SD_CARD	1

#define MAX31875_TEMP_MASK_NORMAL	NO_OS_GENMASK(14, 4)
#define MAX31875_TEMP_MASK_EXTENDED	NO_OS_GENMASK(14, 3)

#define CN0537_MODULE_CASE_PLACEHOLDER1 0
#define CN0537_MODULE_CASE_PLACEHOLDER2 1

#define CN0537_EFUSE_ECC_NOERR		0
#define CN0537_EFUSE_ECC_DATAERR	1
#define CN0537_EFUSE_ECC_PARITYERR	2
#define CN0537_EFUSE_ECC_MANYERR	3

#define CN0537_PDET_AMPS_PER_WATT_BLUE		0.26
#define CN0537_PDET_AMPS_PER_WATT_IR		0.41
#define CN0537_NOMINAL_WATT_PER_AMP_BLUE	0.38
#define CN0537_NOMINAL_WATT_PER_AMP_IR		0.22
#define CN0537_BLUE_DFA0			0.98976
#define CN0537_BLUE_DFA1			-0.0051448
#define CN0537_BLUE_DFA2			0.000020287
#define CN0537_BLUE_DFA3			-0.000000029645
#define CN0537_LED_DERATING_FACTOR_BLUE(x)	(CN0537_BLUE_DFA0 + \
						CN0537_BLUE_DFA1 * (x) + \
						CN0537_BLUE_DFA2 * (x) * (x) + \
						CN0537_BLUE_DFA3 * (x) * (x) * (x))
#define CN0537_LED_DERATING_FACTOR_IR		1.0

#define ALARM_ENABLED				0

#define WINDOW_SIZE				3
/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static float filter_window[WINDOW_SIZE];
static float filter_window_sorted[WINDOW_SIZE];
static bool filter_initialized;

struct max_i2c_init_param i2c_extra_param = {
	.vssel = 0
};

/** CLI command functions table */
static cmd_func app_cli_cmd[] = {
	(cmd_func)cn0537_reg_read,
	(cmd_func)cn0537_reg_write,
	(cmd_func)cn0537_stream,
	(cmd_func)cn0537_idle,
	(cmd_func)cn0537_help,
	(cmd_func)cn0537_mode_set,
	(cmd_func)cn0537_register_dump,
	(cmd_func)cn0537_odr_get,
	(cmd_func)cn0537_odr_set,
	(cmd_func)cn0537_func_test,
	(cmd_func)cn0537_note,
	(cmd_func)cn0537_file_open,
	(cmd_func)cn0537_file_close,
	NULL
};

/** CLI command strings table */
static uint8_t *app_cli_ascii_cmd[] = {
	(uint8_t *)"reg_read ",
	(uint8_t *)"rr ",
	(uint8_t *)"reg_write ",
	(uint8_t *)"rw ",
	(uint8_t *)"stream",
	(uint8_t *)"s",
	(uint8_t *)"idle",
	(uint8_t *)"i",
	(uint8_t *)"help",
	(uint8_t *)"h",
	(uint8_t *)"mode_set ",
	(uint8_t *)"ms ",
	(uint8_t *)"register_dump",
	(uint8_t *)"rd",
	(uint8_t *)"odr_get",
	(uint8_t *)"og",
	(uint8_t *)"odr_set ",
	(uint8_t *)"os ",
	(uint8_t *)"test",
	(uint8_t *)"t",
	(uint8_t *)"note ",
	(uint8_t *)"n ",
	(uint8_t *)"file_open ",
	(uint8_t *)"fo ",
	(uint8_t *)"file_close",
	(uint8_t *)"fc",
	(uint8_t *)"reset",
	(uint8_t *)"rst",
	(uint8_t *)""
};

/** CLI command strings' lengths table */
static uint8_t app_cli_sizes[] = {
	9, 3, 10, 3, 6, 1, 4, 1, 5, 2, 9, 3, 14, 3, 8, 3, 8, 3, 5, 2,
	5, 2, 10, 3, 11, 2, 5, 3
};

/** SD card handler structure */
struct sd_desc *sd_desc;

/** eFuse register values */
static uint8_t led1_gain_coeff, led3_gain_coeff, led1_int_coeff, led3_int_coeff,
       module_type;

/** Calibration variables */
static float blue_led_wpera, ir_led_wpera, blue_na_per_lsb, ir_na_per_lsb;
static float blue_led_curr, ir_led_curr;
static float blue_ptr_coeff, ir_ptr_coeff;
static uint32_t blue_pulse_no, ir_pulse_no;

/** Stream modes */
static char *cn0537_modes[] = {
	"CODE",
	"PTR",
	"RAW",
	""
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

int sort_compare_fn(const void *a, const void *b) {
	return *(float *)a - *(float *)b;
}

static void cn0537_window_init(float element)
{
	for (int i = 0; i < WINDOW_SIZE; i++)
		filter_window[i] = element;
}

static void cn0537_window_push(float element)
{
	for (int i = WINDOW_SIZE - 1; i > 0; i--) {
		filter_window[i] = filter_window[i - 1];
	}

	filter_window[0] = element;
}

static float cn0537_window_median()
{
	for (int i = 0; i < WINDOW_SIZE; i++)
		filter_window_sorted[i] = filter_window[i];

	qsort(filter_window_sorted, WINDOW_SIZE, sizeof(filter_window_sorted[0]), sort_compare_fn);

	return filter_window_sorted[WINDOW_SIZE / 2];
}

/**
 * @brief Read a temperature sample from the MAX31875 sensor
 * @param dev - MAX31875 descriptor
 * @param extended - Choose between normal and extended mode
 * @param temp - Temperature value
 * @return 0 in the case of success, errno errors otherwise
 */
static int32_t max31875_get_single_temp(struct max31875_dev *dev,
					uint8_t extended, float *temp)
{
	float lsb_value = 0.0625;
	uint32_t temp_reg_val = 0;
	int16_t temp_val;
	int32_t ret;

	ret = max31875_reg_read(dev, MAX31875_TEMPERATURE_REG, &temp_reg_val);
	if (ret)
		return ret;

	if (extended) {
		temp_reg_val = no_os_field_get(NO_OS_GENMASK(15, 3), temp_reg_val);
		temp_val = no_os_sign_extend16(temp_reg_val, 12);
	} else {
		temp_reg_val = no_os_field_get(NO_OS_GENMASK(15, 4), temp_reg_val);
		temp_val = no_os_sign_extend16(temp_reg_val, 11);
	}

	*temp = temp_val * lsb_value;

	return 0;
}

/**
 * @brief Return the default initial configuration for the application.
 * @param init_param - Initialization structure to be populated with default
 *                     values.
 * @return void
 */
void cn0537_get_config(struct cn0537_init_param *init_param)
{
	struct max_uart_init_param *extra_uart_param;
	struct max_spi_init_param *spi_extra_param;
	struct max_gpio_init_param *gpio_out_param;
	struct max_gpio_init_param *gpio_in_param;

	extra_uart_param = calloc(1, sizeof(*extra_uart_param));
	if (!extra_uart_param)
		return;

	spi_extra_param = calloc(1, sizeof(*spi_extra_param));
	if (!extra_uart_param)
		return;
		
	gpio_out_param = calloc(1, sizeof(*gpio_out_param));
	if (!extra_uart_param)
		return;
		
	gpio_in_param = calloc(1, sizeof(*gpio_in_param));
	if (!extra_uart_param)
		return;

	extra_uart_param->flow = UART_FLOW_DIS;
	gpio_out_param->vssel = MXC_GPIO_VSSEL_VDDIOH;
	gpio_in_param->vssel = MXC_GPIO_VSSEL_VDDIO;

	init_param->max31875_init.i2c_init.device_id = 0;
	init_param->max31875_init.i2c_init.platform_ops = &max_i2c_ops;
	init_param->max31875_init.i2c_init.max_speed_hz = 100000;
	init_param->max31875_init.i2c_init.extra = &i2c_extra_param;
	init_param->max31875_init.i2c_init.slave_address = 72;

	init_param->adpd188_init.device = ADPD1080;
	init_param->adpd188_init.phy_opt = ADPD188_I2C;
	init_param->adpd188_init.phy_init.i2c_phy.device_id = 0;
	init_param->adpd188_init.phy_init.i2c_phy.max_speed_hz = 400000;
	init_param->adpd188_init.phy_init.i2c_phy.slave_address = 0x64;
	init_param->adpd188_init.phy_init.i2c_phy.platform_ops = &max_i2c_ops;
	init_param->adpd188_init.phy_init.i2c_phy.extra = &i2c_extra_param;

	/**
	 * UART 1 is used for CLI interaction.
	 */
	init_param->cli_init.uart_init.device_id = 1;
	init_param->cli_init.uart_init.baud_rate = 115200;
	init_param->cli_init.uart_init.size = NO_OS_UART_CS_8;
	init_param->cli_init.uart_init.parity = NO_OS_UART_PAR_NO;
	init_param->cli_init.uart_init.platform_ops = &max_uart_ops;
	init_param->cli_init.uart_init.asynchronous_rx = false;
	init_param->cli_init.uart_init.irq_id = 1;
	init_param->cli_init.uart_init.stop = NO_OS_UART_STOP_2_BIT;
	init_param->cli_init.uart_init.extra = extra_uart_param;

	/**
	 * Global (NVIC) interrupt controller.
	 */

	init_param->irq_init.irq_ctrl_id = 0;
	init_param->irq_init.platform_ops = &max_irq_ops;
	init_param->irq_init.extra = NULL;

	/**
	 * GPIO0 and GPIO1  of the ADPD188
	 */
	init_param->adpd188_init.gpio0_init.port = 0;
	init_param->adpd188_init.gpio0_init.extra = gpio_in_param;
	init_param->adpd188_init.gpio0_init.platform_ops = &max_gpio_ops;
	init_param->adpd188_init.gpio0_init.number = 2;
	init_param->adpd188_init.gpio0_init.pull = NO_OS_PULL_UP;
	init_param->adpd188_init.gpio1_init.port = 0;
	init_param->adpd188_init.gpio1_init.extra = gpio_in_param;
	init_param->adpd188_init.gpio1_init.platform_ops = &max_gpio_ops;
	init_param->adpd188_init.gpio1_init.pull = NO_OS_PULL_UP;
	init_param->adpd188_init.gpio1_init.number = 3;

	init_param->rtc_init.id = 0;
	init_param->rtc_init.freq = 0;
	init_param->rtc_init.load = 0;

	/**
	 * This is high while the alarm is triggered.
	 */
	init_param->gpio_alrm.port = 0;
	init_param->gpio_alrm.number = 5;
	init_param->gpio_alrm.pull = NO_OS_PULL_NONE;
	init_param->gpio_alrm.platform_ops = &max_gpio_ops;
	init_param->gpio_alrm.extra = gpio_out_param;

	/**
	 * The rest of the GPIOs are particular to CN0537 board.
	 */
	init_param->gpio_btn.port = 0;
	init_param->gpio_btn.number = 0;
	init_param->gpio_btn.pull = NO_OS_PULL_NONE;
	init_param->gpio_btn.platform_ops = &max_gpio_ops;
	init_param->gpio_btn.extra = gpio_in_param;

	init_param->gpio_cup1.port = 0;
	init_param->gpio_cup1.number = 4;
	init_param->gpio_cup1.platform_ops = &max_gpio_ops;
	init_param->gpio_cup1.pull = NO_OS_PULL_NONE;
	init_param->gpio_cup1.extra = gpio_out_param;
}

/**
 * @brief Generate Hamming parity for a block of data.
 * @param data - Pointer to the data block.
 * @return Hamming parity.
 */
static int8_t generate_hamm_block_parity(uint8_t *data)
{
	const uint8_t paritymap[112] = {
		131, 133, 134, 135, 137, 138, 139, 140, 141, 142,
		143, 145, 146, 147, 148, 149, 150, 151, 152, 153,
		154, 155, 156, 157, 158, 159, 161, 162, 163, 164,
		165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
		175, 176, 177, 178, 179, 180, 181, 182, 183, 184,
		185, 186, 187, 188, 189, 190, 191, 193, 194, 195,
		196, 197, 198, 199, 200, 201, 202, 203, 204, 205,
		206, 207, 208, 209, 210, 211, 212, 213, 214, 215,
		216, 217, 218, 219, 220, 221, 222, 223, 224, 225,
		226, 227, 228, 229, 230, 231, 232, 233, 234, 235,
		236, 237, 238, 239, 240, 241, 242, 243, 244, 245,
		246, 247
	};
	int8_t bit,byte;
	int8_t h = 0;

	for(byte = 0; byte < 14; byte++) {
		for(bit = 0; bit < 8; bit++) {
			if( (data[byte] & (1<<bit) )!=0) {
				h ^= paritymap[(byte << 3) + bit];
			}
		}
	}

	return h;
}

/**
 * @brief Generate hamming syndrome.
 * @param data - Pointer to the data block in which to generate syndrome.
 * @param parity_in - Parity byte.
 * @return Hamming parity.
 */
static int8_t generate_hamm_syndrome(uint8_t *data, uint8_t *parity_in)
{
	int8_t bit, h;

	h=generate_hamm_block_parity(data);

	for(bit=0; bit<7; bit++)
		if ((*parity_in&(1<<bit))==(1<<bit))
			h^=0x80;

	return h;
}

/**
 * @brief Check and correct if possible data and parity.
 * @param data - Pointer to data block.
 * @param parity - Pointer to parity byte.
 * @return Result of the detection and correction algorithm:
 *         - CN0537_EFUSE_ECC_NOERR if no error was found.
 *         - CN0537_EFUSE_ECC_DATAERR in one bit error in data was found and
 *           corrected
 *         - CN0537_EFUSE_ECC_PARITYERR if one bit error was found and corrected
 *           in the parity byte
 *         - CN0537_EFUSE_ECC_MANYERR if mltiple errors were found and cannot be
 *           corrected
 */
static int32_t fix_hamm_parity(uint8_t *data, uint8_t *parity)
{
	uint8_t calculated_parity;
	uint8_t syn, glob;
	uint8_t bit, byte;
	calculated_parity = generate_hamm_syndrome(data, parity);
	syn = (*parity ^ calculated_parity) & 0x7f;
	glob = (*parity ^ calculated_parity) & 0x80;
	if(glob == 0) {
		if (syn == 0)
			return 0;
		else
			return 3;
	} else {
		if (syn >= 120)
			return 3;
		switch (syn) {
		case 0:
			*parity = *parity ^ 0x80;

			return 2;
		case 1:
			*parity = *parity ^ 0x01;

			return 2;
		case 2:
			*parity = *parity ^ 0x02;

			return 2;
		case 4:
			*parity = *parity ^ 0x04;

			return 2;
		case 8:
			*parity = *parity ^ 0x08;

			return 2;
		case 16:
			*parity = *parity ^ 0x10;

			return 2;
		case 32:
			*parity = *parity ^ 0x20;

			return 2;
		case 64:
			*parity = *parity ^ 0x40;

			return 2;
		default:
			syn = (syn>64) ? syn - 8 :
			      (syn>32) ? syn - 7 :
			      (syn>16) ? syn - 6 :
			      (syn>8) ? syn - 5 :
			      (syn>4) ? syn - 4 : 0;
			byte = syn >> 3;
			bit = syn & 0x7;
			data[byte] = data[byte] ^ (1 << bit);

			return 1;
		}
	}
}

/**
 * @brief Get all eFuse registers from the device.
 * @param dev - Pointer to the application handler.
 * @param data - Pointer to data storage.
 * @return Result of the detection and correction algorithm:
 *         - CN0537_EFUSE_ECC_NOERR if no error was found.
 *         - CN0537_EFUSE_ECC_DATAERR in one bit error in data was found and
 *           corrected
 *         - CN0537_EFUSE_ECC_PARITYERR if one bit error was found and corrected
 *           in the parity byte
 *         - CN0537_EFUSE_ECC_MANYERR if mltiple errors were found and cannot be
 *           corrected
 */
static int32_t cn0537_read_efuse_data_ecc(struct cn0537_dev *dev, uint8_t *data)
{
	uint8_t ecc, i;
	int32_t ret;
	uint16_t reg_data;

	for(i = 0; i < 14; i++) {
		ret = adpd188_reg_read(dev->adpd188_handler, 0x70 + i,
				       &reg_data);
		if (ret != 0)
			return -1;
		data[i] = reg_data & 0xFF;
	}
	ret = adpd188_reg_read(dev->adpd188_handler, 0x7E, &reg_data);
	if (ret != 0)
		return -1;
	ecc = reg_data & 0xFF;

	return fix_hamm_parity(data, &ecc);
}

/**
 * @brief Get device calibration data from the eFuse registers.
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_get_efuse(struct cn0537_dev *dev)
{
	int32_t ret;
	uint16_t reg_data;
	uint16_t reg_data_current;
	uint8_t data[14];
	uint8_t buff[50];

	/** Enable 32kHz clock */
	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_SAMPLE_CLK,
			       &reg_data);
	if (ret != 0)
		return -1;
	reg_data |= ADPD188_SAMPLE_CLK_CLK32K_EN_MASK;
	ret = adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_SAMPLE_CLK,
				reg_data);
	if (ret != 0)
		return -1;

	ret = adpd188_mode_set(dev->adpd188_handler, ADPD188_PROGRAM);
	if (ret != 0)
		return -1;

	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_DATA_ACCESS_CTL, &reg_data);
	if (ret != 0)
		return -1;
	reg_data |= ADPD188_DATA_ACCESS_CTL_DIGITAL_CLOCK_ENA_MASK;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_DATA_ACCESS_CTL, reg_data);
	if (ret != 0)
		return -1;

	/** Enable eFuse registers */
	ret = adpd188_reg_write(dev->adpd188_handler, 0x57, 0x7);
	if (ret != 0)
		return -1;

	/** Wait for eFuse registers to refresh */
	do {
		ret = adpd188_reg_read(dev->adpd188_handler, 0x67, &reg_data);
		if (ret != 0)
			return -1;
	} while (reg_data != 0x4);

	ret = cn0537_read_efuse_data_ecc(dev, data);
	if (ret == CN0537_EFUSE_ECC_MANYERR) {
		sprintf((char *)buff, "ERROR: eFuze registers corrupted.\n");
		no_os_uart_write(dev->cli_handler->uart_device, buff,
			   strlen((char *)buff));

		return -1;
	}
	sprintf((char *)buff, "Module type %x\n", data[0]);
	no_os_uart_write(dev->cli_handler->uart_device, buff, strlen((char *)buff));

	module_type = data[0];
	led1_gain_coeff = data[1];
	led3_gain_coeff = data[2];
	led1_int_coeff = data[3];
	led3_int_coeff = data[4];

	/* Calibrate the frequency of the 32 kHz and 32 MHz clocks as described in AN-2033 */
	if (module_type == 0x21){
		ret = adpd188_reg_read(dev->adpd188_handler, 0x77, &reg_data);
		if (ret)
			return -1;

		ret = adpd188_reg_read(dev->adpd188_handler, 0x4B, &reg_data_current);
		if (ret)
			return -1;

		reg_data_current &= ~NO_OS_GENMASK(8, 0);
		reg_data_current |= reg_data;

		ret = adpd188_reg_write(dev->adpd188_handler, 0x4B, reg_data_current);
		if (ret)
			return -1;

		ret = adpd188_reg_read(dev->adpd188_handler, 0x78, &reg_data);
		if (ret)
			return -1;

		ret = adpd188_reg_read(dev->adpd188_handler, 0x4D, &reg_data_current);
		if (ret)
			return -1;

		reg_data_current &= ~NO_OS_GENMASK(8, 0);
		reg_data_current |= reg_data;

		ret = adpd188_reg_write(dev->adpd188_handler, 0x4D, reg_data_current);
		if (ret)
			return -1;
	}

	/** Disable eFuse registers */
	ret = adpd188_reg_write(dev->adpd188_handler, 0x57, 0x0);
	if (ret != 0)
		return -1;

	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_DATA_ACCESS_CTL, &reg_data);
	if (ret != 0)
		return -1;
	reg_data &= ~ADPD188_DATA_ACCESS_CTL_DIGITAL_CLOCK_ENA_MASK;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_DATA_ACCESS_CTL, reg_data);
	if (ret != 0)
		return -1;

	return adpd188_mode_set(dev->adpd188_handler, ADPD188_STANDBY);
}

/**
 * @brief Do ADPD device hardware initialization routine.
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_init_adpdinit(struct cn0537_dev *dev)
{
	int32_t ret;
	uint16_t reg_data;
	uint8_t buff[50];
	struct adpd188_slot_config slot_a, slot_b;

	ret = cn0537_get_efuse(dev);
	if (ret != 0)
		return -1;

	/** Enable 32kHz clock */
	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_SAMPLE_CLK,
			       &reg_data);
	if (ret != 0)
		return -1;
	reg_data |= ADPD188_SAMPLE_CLK_CLK32K_EN_MASK;
	ret = adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_SAMPLE_CLK,
				reg_data);
	if (ret != 0)
		return -1;

	ret = adpd188_mode_set(dev->adpd188_handler, ADPD188_PROGRAM);
	if (ret != 0)
		return -1;

	sprintf((char *)buff, "ADPD188BI device setup error.\n");
	ret = adpd188_smoke_detect_setup(dev->adpd188_handler);
	if (ret != 0) {
		no_os_uart_write(dev->cli_handler->uart_device, buff,
			   strlen((char *)buff));
		return -1;
	}

	/** Set sample rate to 1/6 SPS */
	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_FSAMPLE,
			       &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0xC000;
	ret = adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_FSAMPLE,
				reg_data);
	if (ret != 0)
		return -1;

	/** Set time slots for 16 bit samples */
	slot_a.slot_id = ADPD188_SLOTA;
	slot_a.slot_en = true;
	slot_a.sot_fifo_mode = ADPD188_32BIT_SUM;
	ret = adpd188_slot_setup(dev->adpd188_handler, slot_a);
	if (ret != 0)
		return -1;
	slot_b.slot_id = ADPD188_SLOTB;
	slot_b.slot_en = true;
	slot_b.sot_fifo_mode = ADPD188_32BIT_SUM;
	ret = adpd188_slot_setup(dev->adpd188_handler, slot_b);
	if (ret != 0)
		return -1;

	/** Set set channels to one PD each */
	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_PD_LED_SELECT,
			       &reg_data);
	if (ret != 0)
		return -1;
	reg_data &= ~(ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_MASK |
		      ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_MASK);
	reg_data |= (1 << ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_MASK;
	reg_data |= (1 << ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_MASK;
	ret = adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_PD_LED_SELECT,
				reg_data);
	if (ret != 0)
		return -1;

	/** Disable channel 1 and let the others active */
	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_AFE_PWR_CFG1,
			       &reg_data);
	if (ret != 0)
		return -1;
	reg_data &= ~ADPD188_AFE_PWR_CFG1_AFE_POWERDOWN_MASK;
	reg_data |= (0x38 << ADPD188_AFE_PWR_CFG1_AFE_POWERDOWN_POS) &
		    ADPD188_AFE_PWR_CFG1_AFE_POWERDOWN_MASK;
	ret = adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_AFE_PWR_CFG1,
				reg_data);
	if (ret != 0)
		return -1;

	/** Set ADC offsets to 0 on the active channels */
	/** Slot A */
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTA_CH1_OFFSET, &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0x0000;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_SLOTA_CH1_OFFSET, reg_data);
	if (ret != 0)
		return -1;
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTA_CH2_OFFSET, &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0x3FFF;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_SLOTA_CH2_OFFSET, reg_data);
	if (ret != 0)
		return -1;
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTA_CH3_OFFSET, &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0x3FFF;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_SLOTA_CH3_OFFSET, reg_data);
	if (ret != 0)
		return -1;
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTA_CH4_OFFSET, &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0x3FFF;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_SLOTA_CH4_OFFSET, reg_data);
	if (ret != 0)
		return -1;
	/** Slot B */
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTB_CH1_OFFSET, &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0x0000;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_SLOTB_CH1_OFFSET, reg_data);
	if (ret != 0)
		return -1;
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTB_CH2_OFFSET, &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0x3FFF;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_SLOTB_CH2_OFFSET, reg_data);
	if (ret != 0)
		return -1;
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTB_CH3_OFFSET, &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0x3FFF;
	ret = adpd188_reg_write(dev->adpd188_handler,
				ADPD188_REG_SLOTB_CH3_OFFSET, reg_data);
	if (ret != 0)
		return -1;
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTB_CH4_OFFSET, &reg_data);
	if (ret != 0)
		return -1;
	reg_data = 0x3FFF;
	return adpd188_reg_write(dev->adpd188_handler,
				 ADPD188_REG_SLOTB_CH4_OFFSET, reg_data);
}

/**
 * @brief Initialize GPIO hardware and handlers.
 * @param dev - Pointer to the application handler.
 * @param init_param - Pointer to the initialization structure.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_gpio_get(struct cn0537_dev *dev,
			       struct cn0537_init_param *init_param)
{
	int32_t ret;

	ret = no_os_gpio_get(&dev->gpio_alrm, &init_param->gpio_alrm);
	if (ret != 0)
		return -1;

	ret = no_os_gpio_direction_output(dev->gpio_alrm, NO_OS_GPIO_LOW);
	if (ret != 0)
		return -1;

	ret = no_os_gpio_get(&dev->gpio_btn, &init_param->gpio_btn);
	if (ret != 0)
		return -1;

	ret = no_os_gpio_direction_input(dev->gpio_btn);
	if (ret != 0)
		return -1;

	ret = no_os_gpio_get(&dev->gpio_cup1, &init_param->gpio_cup1);
	if (ret != 0)
		return -1;

	return no_os_gpio_direction_output(dev->gpio_cup1, NO_OS_GPIO_LOW);
}

/**
 * @brief Free memory allocated by cn0537_gpio_get().
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_gpio_rmv(struct cn0537_dev *dev)
{
	int32_t ret;

	if (dev->gpio_alrm) {
		ret = no_os_gpio_remove(dev->gpio_alrm);
		if (ret != 0)
			return -1;
	}

	if (dev->gpio_btn) {
		ret = no_os_gpio_remove(dev->gpio_btn);
		if (ret != 0)
			return -1;
	}

	if (dev->gpio_cup1) {
		ret = no_os_gpio_remove(dev->gpio_cup1);
		if (ret != 0)
			return -1;
	}

	return 0;
}

/**
 * @brief Calculate PTR measurements from codes.
 * @param dev - Pointer to the application handler.
 * @return void
 */
static void cn0537_get_ptr(struct cn0537_dev *dev)
{
	dev->blue_ptr = dev->blue_data * blue_ptr_coeff;
	dev->ir_ptr = dev->ir_data * ir_ptr_coeff;
}

/**
 * @brief Read data from the sensor.
 * @param dev - Pointer to the application handler.
 * @param data_rdy - true if new data is available, false if new data is not
 *                   ready.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_process_get_data(struct cn0537_dev *dev, bool *data_rdy)
{
	int32_t ret = 0;
	uint8_t byte_no, i;
	uint16_t data_buff[(2 * CN0537_TIMESLOTS_ACTIVE)];

	const float ir_temp_coff[] = {
		1.023, 1.024, 1.024, 1.024, 1.023, 1.022, 1.020, 1.018,
		1.016, 1.014, 1.011, 1.007, 1.004, 1, 0.996, 0.992,
		0.987, 0.983, 0.978, 0.973, 0.968, 0.963, 0.958, 0.952,
		0.947, 0.942
	};
	const float blue_temp_coff_100ma[] = {
		0.932, 0.947, 0.96, 0.971, 0.98, 0.988, 0.994, 0.998,
		1.001, 1.003, 1.004, 1.004, 1.003, 1, 0.998, 0.994,
		0.99, 0.986, 0.981, 0.976, 0.971, 0.966, 0.961, 0.956,
		0.951, 0.947
	};
	const float blue_temp_coff_175ma[] = {
		0.875, 0.894, 0.912, 0.927, 0.941, 0.953, 0.963, 0.972,
		0.980, 0.986, 0.991, 0.995, 0.998, 1, 1.001, 1.001,
		1.001, 0.999, 0.998, 0.995, 0.992, 0.989, 0.986, 0.982,
		0.979, 0.975
	};
	uint8_t coff_index;

	*data_rdy = false;

	ret = adpd188_fifo_status_get(dev->adpd188_handler, &byte_no);
	if (ret)
		goto out;

	if (byte_no < 16)
		goto out;

	for (i = 0; i < (2 * CN0537_TIMESLOTS_ACTIVE); i++) {
		ret = adpd188_reg_read(dev->adpd188_handler,
				       ADPD188_REG_FIFO_ACCESS, (data_buff + i));
		if (ret)
			goto out;
	}

	dev->blue_data = data_buff[0];
	dev->blue_data |= data_buff[1] << 16;
	dev->ir_data = data_buff[2];
	dev->ir_data |= data_buff[3] << 16;

	dev->blue_raw = dev->blue_data;
	dev->ir_raw = dev->ir_data;

	ret = max31875_get_single_temp(dev->max31875_handler, 0, &dev->temp_celsius);
	if (ret)
		goto out;

	coff_index = (40 + dev->temp_celsius) / 5;
	/* Make sure we're not out of array bounds */
	if (coff_index >= NO_OS_ARRAY_SIZE(ir_temp_coff))
		coff_index = NO_OS_ARRAY_SIZE(ir_temp_coff) - 1;

	dev->ir_data /= ir_temp_coff[coff_index];
	if (blue_led_curr > 175)
		dev->blue_data /= blue_temp_coff_175ma[coff_index];
	else if (100 > blue_led_curr)
		dev->blue_data /= blue_temp_coff_100ma[coff_index];
	else if (fabs(175 - blue_led_curr) < fabs(100 - blue_led_curr))
		dev->blue_data /= blue_temp_coff_175ma[coff_index];
	else
		dev->blue_data /= blue_temp_coff_100ma[coff_index];

	cn0537_get_ptr(dev);

	if (!filter_initialized) {
		cn0537_window_init(dev->ir_ptr);
		filter_initialized = true;
	} else {
		cn0537_window_push(dev->ir_ptr);
		dev->ir_ptr_median = cn0537_window_median();
	}

	dev->timestamp = no_os_get_time();

	ret = no_os_rtc_get_cnt(dev->rtc_handler, &dev->rtc_count);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto out;

	*data_rdy = true;

out:

	return ret;
}

/**
 * @brief Calculate LED current from device registers.
 * @param dev - Pointer to the application handler.
 * @param led_no - LED ID.
 * @param curr - Pointer to the current value container.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_get_led_curr(struct cn0537_dev *dev, uint8_t led_no,
				   float *curr)
{
	int32_t ret;
	uint16_t reg_data;
	float coarse, scale, fine;

	switch (led_no) {
	case 1:
		ret = adpd188_reg_read(dev->adpd188_handler,
				       ADPD188_REG_ILED1_COARSE, &reg_data);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
		coarse = 50.3 + 19.8 * (float)((reg_data &
						ADPD188_ILED1_COARSE_ILED1_COARSE_MASK) >>
					       ADPD188_ILED1_COARSE_ILED1_COARSE_POS);
		scale = 0.1 + 0.9 * (reg_data &
				     ADPD188_ILED1_COARSE_ILED1_SCALE_MASK) ? 1 : 0;
		ret = adpd188_reg_read(dev->adpd188_handler,
				       ADPD188_REG_ILED_FINE, &reg_data);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
		fine = 0.74 + 0.022 * ((reg_data &
					ADPD188_ILED_FINE_ILED1_FINE_MASK) >>
				       ADPD188_ILED_FINE_ILED1_FINE_POS);
		break;
	case 2:
		ret = adpd188_reg_read(dev->adpd188_handler,
				       ADPD188_REG_ILED2_COARSE, &reg_data);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
		coarse = 50.3 + 19.8 * (float)((reg_data &
						ADPD188_ILED2_COARSE_ILED2_COARSE_MASK) >>
					       ADPD188_ILED2_COARSE_ILED2_COARSE_POS);
		scale = 0.1 + 0.9 * (reg_data &
				     ADPD188_ILED2_COARSE_ILED2_SCALE_MASK) ? 1 : 0;
		ret = adpd188_reg_read(dev->adpd188_handler,
				       ADPD188_REG_ILED_FINE, &reg_data);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
		fine = 0.74 + 0.022 * ((reg_data &
					ADPD188_ILED_FINE_ILED2_FINE_MASK) >>
				       ADPD188_ILED_FINE_ILED2_FINE_POS);
		break;
	case 3:
		ret = adpd188_reg_read(dev->adpd188_handler,
				       ADPD188_REG_ILED3_COARSE, &reg_data);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
		coarse = 50.3 + 19.8 * (float)((reg_data &
						ADPD188_ILED3_COARSE_ILED3_COARSE_MASK) >>
					       ADPD188_ILED3_COARSE_ILED3_COARSE_POS);
		scale = 0.1 + 0.9 * (reg_data &
				     ADPD188_ILED3_COARSE_ILED3_SCALE_MASK) ? 1 : 0;
		ret = adpd188_reg_read(dev->adpd188_handler,
				       ADPD188_REG_ILED_FINE, &reg_data);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
		fine = 0.74 + 0.022 * ((reg_data &
					ADPD188_ILED_FINE_ILED3_FINE_MASK) >>
				       ADPD188_ILED_FINE_ILED3_FINE_POS);
		break;
	default:
		return -1;
	}

	*curr = coarse * fine * scale;

	return 0;
}

/**
 * @brief Update LED power/current ratio. Used to calculate PTR measurements.
 * @param dev - Pointer to the application handler.
 * @return void
 */
static int32_t cn0537_stream_update_led_wpera(struct cn0537_dev *dev)
{
	float gain_cal_blue, gain_cal_ir;
	float blue_gain, ir_gain, blue_inter, ir_inter;
	float blue_nominal_scalar, ir_nominal_scalar;

	switch (module_type) {
	case 30:
	case 31:
		blue_gain = (17.0 / 256.0) * (led1_gain_coeff - 112) + 17.0;
		blue_inter = (float)(8.0 * (led1_int_coeff - 128));
		blue_nominal_scalar = 17.0 * blue_led_curr + 622.0;

		ir_gain = (34.0 / 256.0) * (led3_gain_coeff - 112) + 34.0;
		ir_inter = (float)(5.0 * (led3_int_coeff - 128));
		ir_nominal_scalar = 34.0 * ir_led_curr + 128.0;
		break;
	case 33:
		blue_gain = (21.0 / 256.0) * (led1_gain_coeff - 112) + 21.0;
		blue_inter = (float)(8.0 * (led1_int_coeff - 80));
		blue_nominal_scalar = 21.0 * blue_led_curr + 753.0;

		ir_gain = (42.0 / 256.0) * (led3_gain_coeff - 112) + 42.0;
		ir_inter = (float)(5.0 * (led3_int_coeff - 80));
		ir_nominal_scalar = 42.0 * ir_led_curr + 156.0;
		break;
	case CN0537_MODULE_CASE_PLACEHOLDER1:
		/* Case for later module, return error since not yet
		 * implemented */
		return -1;
	case CN0537_MODULE_CASE_PLACEHOLDER2:
		/* Case for later module, return error since not yet
		 * implemented */
		return -1;
	default:
		return -1;
	}

	gain_cal_blue = (blue_gain * blue_led_curr + blue_inter) /
			blue_nominal_scalar;
	blue_led_wpera = CN0537_NOMINAL_WATT_PER_AMP_BLUE *
			 CN0537_LED_DERATING_FACTOR_BLUE(blue_led_curr) *
			 gain_cal_blue;

	gain_cal_ir = (ir_gain * ir_led_curr + ir_inter) / ir_nominal_scalar;
	ir_led_wpera = CN0537_NOMINAL_WATT_PER_AMP_IR *
		       CN0537_LED_DERATING_FACTOR_IR * gain_cal_ir;

	return 0;
}

/**
 * @brief Update LED pulse number. Used to calculate PTR measurements.
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_stream_update_pulseno(struct cn0537_dev *dev)
{
	int32_t ret;
	uint16_t reg_data;

	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTA_NUMPULSES, &reg_data);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;
	blue_pulse_no = (reg_data &
			 ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_MASK) >>
			ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_POS;

	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTB_NUMPULSES, &reg_data);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;
	ir_pulse_no = (reg_data & ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_MASK) >>
		      ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_POS;

	return 0;
}

/**
 * @brief Update ADC nano-amperes/LSB ratio. Used to calculate PTR measurements.
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_stream_update_naperlsb(struct cn0537_dev *dev)
{
	int32_t ret;
	uint16_t gain_opt, pulse_width;

	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_SLOTA_TIA_CFG,
			       &gain_opt);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;
	gain_opt &= ADPD188_SLOTA_TIA_CFG_SLOTA_TIA_GAIN_MASK;
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTA_LED_PULSE, &pulse_width);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;
	pulse_width = (pulse_width &
		       ADPD188_SLOTA_LED_PULSE_SLOTA_LED_WIDTH_MASK) >>
		      ADPD188_SLOTA_LED_PULSE_SLOTA_LED_WIDTH_POS;
	switch (gain_opt) {
	case 0:
		if (pulse_width == 3)
			blue_na_per_lsb = 0.41;
		else if (pulse_width == 2)
			blue_na_per_lsb = 0.58;
		else
			return -1;
		break;
	case 1:
		if (pulse_width == 3)
			blue_na_per_lsb = 0.82;
		else if (pulse_width == 2)
			blue_na_per_lsb = 1.15;
		else
			return -1;
		break;
	case 2:
		if (pulse_width == 3)
			blue_na_per_lsb = 1.64;
		else if (pulse_width == 2)
			blue_na_per_lsb = 2.31;
		else
			return -1;
		break;
	case 3:
		if (pulse_width == 3)
			blue_na_per_lsb = 3.27;
		else if (pulse_width == 2)
			blue_na_per_lsb = 4.62;
		else
			return -1;
		break;
	}

	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_SLOTB_TIA_CFG,
			       &gain_opt);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;
	gain_opt &= ADPD188_SLOTB_TIA_CFG_SLOTB_TIA_GAIN_MASK;
	ret = adpd188_reg_read(dev->adpd188_handler,
			       ADPD188_REG_SLOTB_LED_PULSE, &pulse_width);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;
	pulse_width = (pulse_width &
		       ADPD188_SLOTB_LED_PULSE_SLOTB_LED_WIDTH_MASK) >>
		      ADPD188_SLOTB_LED_PULSE_SLOTB_LED_WIDTH_POS;
	switch (gain_opt) {
	case 0:
		if (pulse_width == 3)
			ir_na_per_lsb = 0.41;
		else if (pulse_width == 2)
			ir_na_per_lsb = 0.58;
		else
			return -1;
		break;
	case 1:
		if (pulse_width == 3)
			ir_na_per_lsb = 0.82;
		else if (pulse_width == 2)
			ir_na_per_lsb = 1.15;
		else
			return -1;
		break;
	case 2:
		if (pulse_width == 3)
			ir_na_per_lsb = 1.64;
		else if (pulse_width == 2)
			ir_na_per_lsb = 2.31;
		else
			return -1;
		break;
	case 3:
		if (pulse_width == 3)
			ir_na_per_lsb = 3.27;
		else if (pulse_width == 2)
			ir_na_per_lsb = 4.62;
		else
			return -1;
		break;
	}

	return 0;
}

/**
 * @brief Wrapper function to update all PTR parameters before a stream command.
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_stream_update_ptr_param(struct cn0537_dev *dev)
{
	const uint8_t ir_led_id = 3, blue_led_id = 1;
	int32_t ret;

	ret = cn0537_get_led_curr(dev, blue_led_id, &blue_led_curr);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;
	ret = cn0537_get_led_curr(dev, ir_led_id, &ir_led_curr);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	ret = cn0537_stream_update_led_wpera(dev);
	if (NO_OS_IS_ERR_VALUE(ret)) 
		return -1;

	ret = cn0537_stream_update_pulseno(dev);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	ret = cn0537_stream_update_naperlsb(dev);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	blue_ptr_coeff = (blue_na_per_lsb * (1.0 / (float)blue_pulse_no)) /
			 (blue_led_curr * blue_led_wpera *
			  CN0537_PDET_AMPS_PER_WATT_BLUE);
	ir_ptr_coeff = (ir_na_per_lsb * (1.0 / (float)ir_pulse_no)) /
		       (ir_led_curr * ir_led_wpera *
			CN0537_PDET_AMPS_PER_WATT_IR);

	return 0;
}

/**
 * @brief Initialize the application.
 * @param device - Pointer to the application handler.
 * @param init_param - Pointer to the initialization structure.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_init(struct cn0537_dev **device,
		    struct cn0537_init_param *init_param)
{
	struct cn0537_dev *dev;
	int32_t ret;
	uint8_t buff[50];
	uint8_t app_name[] = {'C', 'N', '0', '5', '8', '3', 0};
	struct no_os_spi_desc *sd_spi_desc;
	FRESULT f_ret;
	struct no_os_callback_desc *cli_cb;
	uint16_t reg_data;
	float temperature;

	dev = (struct cn0537_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	dev->stream_en = false;
	dev->stream_mode = CN0537_PTR;
	dev->stream_cnt_en = false;
	dev->idle = 1;
	dev->file_open = false;

	ret = no_os_irq_ctrl_init(&dev->irq_handler, &init_param->irq_init);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_cli;

	ret = cli_setup(&(dev->cli_handler), &(init_param->cli_init));
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_dev;

	cli_load_command_vector(dev->cli_handler, app_cli_cmd);
	cli_load_command_calls(dev->cli_handler, app_cli_ascii_cmd);
	cli_load_command_sizes(dev->cli_handler, app_cli_sizes);
	cli_load_descriptor_pointer(dev->cli_handler, dev);

	cli_cb = calloc(1, sizeof(*cli_cb));
	cli_cb->callback = cli_rx_callback;
	cli_cb->event = NO_OS_EVT_UART_RX_COMPLETE;
	cli_cb->peripheral = NO_OS_UART_IRQ;
	cli_cb->ctx = dev->cli_handler;
	cli_cb->handle = MXC_UART1;

	ret = no_os_irq_global_enable(dev->irq_handler);
	ret = no_os_irq_register_callback(dev->irq_handler, UART1_IRQn, cli_cb);
	ret = no_os_irq_enable(dev->irq_handler, UART1_IRQn);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_irq;

	ret = adpd188_init(&dev->adpd188_handler, &init_param->adpd188_init);
	ret = cn0537_init_adpdinit(dev);

	ret = max31875_init(&dev->max31875_handler, &init_param->max31875_init);
	ret = max31875_get_single_temp(dev->max31875_handler, 0, &temperature);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		no_os_uart_write(dev->cli_handler->uart_device, buff,
			   strlen((char *)buff));
		goto error_irq;
	}
	
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_adpd;
	ret = no_os_rtc_init(&dev->rtc_handler, &init_param->rtc_init);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_adpd;

	ret = no_os_rtc_start(dev->rtc_handler);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_rtc;

	struct max_spi_init_param sd_spi_extra = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = 1,
	};

	/**
	 * SPI parameters for SD card interface. 
	 * The card cannot be used if the ADPD188 uses the SPI interface,
	 * since the MAX32660 only has 1 CS pin.
	 */
	struct no_os_spi_init_param sd_spi_init_param = {
		.device_id = 1,
		.chip_select = 0,
		.max_speed_hz = 600000,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.extra = &sd_spi_extra
	};

	ret = no_os_spi_init(&sd_spi_desc, &sd_spi_init_param);
	if (ret)
		goto error_rtc;

	init_param->sd_init.spi_desc = sd_spi_desc;
	ret = sd_init(&dev->sd_handler, &init_param->sd_init);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"SD card not found.\n\n>");
		if(ret != 0)
			return ret;
		free(init_param->sd_init.spi_desc);
		init_param->sd_init.spi_desc = NULL;
	} else {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"SD card found.\n\n>");
		if(ret != 0)
			return ret;
	}
	sd_desc = dev->sd_handler;

	memset(&dev->ff_obj, 0, sizeof(dev->ff_obj));
	if (init_param->sd_init.spi_desc) {
		f_ret = f_mount(&dev->ff_obj, "0:", 0);
		if (f_ret != FR_OK) {
			ret = cli_write_string(dev->cli_handler,
					       (uint8_t*)"File system not mounted.\n");
			if(ret != 0)
				return ret;
		} else {
			ret = cli_write_string(dev->cli_handler,
					       (uint8_t*)"File system mounted.\n");
			if(ret != 0)
				return ret;
		}
	}

	/** Set the sample rate to 1/6 Hz */
	reg_data = 0xC000;
	ret = adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_FSAMPLE,
				reg_data);
	if (ret != 0)
		return -1;

	/** Resume sampling */
	ret = adpd188_mode_set(dev->adpd188_handler, ADPD188_NORMAL);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	cn0537_gpio_get(dev, init_param);

	ret = cli_cmd_prompt(dev->cli_handler, app_name);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_gpio;
	*device = dev;

	adpd188_fifo_clear(dev->adpd188_handler);

	return 0;

error_gpio:
	cn0537_gpio_rmv(dev);
error_rtc:
	no_os_rtc_remove(dev->rtc_handler);
error_adpd:
	adpd188_remove(dev->adpd188_handler);
error_irq:
	no_os_irq_ctrl_remove(dev->irq_handler);
error_cli:
	cli_remove(dev->cli_handler);
error_dev:
	free(dev);

	return -1;
}

/**
 * @brief Free memory allocated by cn0537_init().
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_remove(struct cn0537_dev *dev)
{
	int32_t ret;

	ret = cli_remove(dev->cli_handler);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	ret = adpd188_remove(dev->adpd188_handler);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	ret = no_os_rtc_remove(dev->rtc_handler);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	ret = sd_remove(dev->sd_handler);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	ret = cn0537_gpio_rmv(dev);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	free(dev);

	return 0;
}

/**
 * @brief Calculate time in seconds using RTC counter readout.
 * @param dev - Pointer to the application handler.
 * @param count - RTC counter value.
 * @param time - Pointer to the time container.
 * @return void
 */
static void cn0537_process_calculate_time(struct cn0537_dev *dev,
		uint32_t count, float *time)
{
	*time = count; 
}

static void cn0537_time_subsecond(struct cn0537_dev *dev,
		struct no_os_time ts, float *time)
{
	*time = ts.s + (float)ts.us / 1000000;
}

/**
 * @brief Print stream data.
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_process_show_data(struct cn0537_dev *dev)
{
	static uint8_t buff_sd[256] __attribute__ ((aligned (4)));
	static uint8_t buff_term[256] __attribute__ ((aligned (4)));
	FRESULT f_ret;
	uint32_t no_written;

	cn0537_time_subsecond(dev, dev->timestamp, &dev->time_stamp);

	if (dev->stream_mode == CN0537_CODE)
		sprintf((char *)buff_term, "\t%.8lX %.8lX %.3f %.3f\n",
			dev->blue_data, dev->ir_data, (double)dev->time_stamp,
			(double)dev->temp_celsius);
	else if (dev->stream_mode == CN0537_PTR)
		sprintf((char *)buff_term, "\t%.5E %.5E %.3f %.3f\n",
			(double)dev->blue_ptr, (double)dev->ir_ptr,
			(double)dev->time_stamp, (double)dev->temp_celsius);
	else if (dev->stream_mode == CN0537_RAW)
		sprintf((char *)buff_term, "\t0x%x 0x%x %.3f %.3f\n",
			dev->blue_raw, dev->ir_raw, (double)dev->time_stamp,
			(double)dev->temp_celsius);
	else
		sprintf((char *)buff_term, "Mode error.");

	cli_write_string(dev->cli_handler, buff_term);

	if (dev->sd_handler->spi_desc && dev->file_open) {
		if (dev->stream_mode == CN0537_CODE)
			sprintf((char *)buff_sd, "\t%.8lX %.8lX %.3f %.3f\n",
				dev->blue_data, dev->ir_data,
				(double)dev->time_stamp,
				(double)dev->temp_celsius);
		else if (dev->stream_mode == CN0537_PTR)
			sprintf((char *)buff_sd, "\t%.5E %.5E %.3f %.3f %.3f\n",
				(double)dev->blue_ptr, (double)dev->ir_ptr,
				(double)dev->ir_ptr_median,
				(double)dev->time_stamp,
				(double)dev->temp_celsius);
		else if (dev->stream_mode == CN0537_RAW)
			sprintf((char *)buff_sd, "\t0x%x 0x%x %.3f %.3f\n",
				dev->blue_raw, dev->ir_raw,
				(double)dev->time_stamp,
				(double)dev->temp_celsius);
		else
			sprintf((char *)buff_sd, "Mode error.");

		f_ret = f_write(&dev->curr_file, buff_sd,
				strlen((char *)buff_sd), (UINT *)&no_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"File write error.\n");
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Control the alarm GPIO.
 * @param dev - Pointer to the application handler.
 * @param enable - true to activate the alarm, false to deactivate it.
 * @return 0 in case of 0, -1 otherwise.
 */
static int32_t cn0537_process_alarm(struct cn0537_dev *dev, bool enable)
{
	if (enable) {
		no_os_gpio_set_value(dev->gpio_cup1, 1);
		no_os_gpio_set_value(dev->gpio_alrm, 1);
	} else {
		no_os_gpio_set_value(dev->gpio_alrm, 0);
		no_os_gpio_set_value(dev->gpio_cup1, 0);
	}

	return 0;
}

/**
 * @brief Application process.
 * @param dev - Pointer to the application handler.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_process(struct cn0537_dev *dev)
{
	int32_t ret;
	bool data_rdy = false;

	ret = cli_process(dev->cli_handler);
	if (ret)
		return ret;

        if (!dev->idle) {
                ret = cn0537_process_get_data(dev, &data_rdy);
                if (ret)
                        return ret;
	}

	if (data_rdy && dev->stream_en && dev->stream_cnt) {
		ret = cn0537_process_show_data(dev);
                if (ret)
                        return ret;
        }

	return 0;
}

/**
 * Help command helper function. Display help function prompt.
 *
 * @param [in] dev	     - The device structure.
 * @param [in] short_command - True to display the long command prompt,
 *                             false to display the short command prompt.
 *
 * @return 0 in case of 0, negative error code otherwise.
 */
static int32_t cn0537_help_prompt(struct cn0537_dev *dev, bool short_command)
{
	int32_t ret;

	if (!short_command) {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"\tCN0537 application.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "For commands with options as arguments typing the command and 'space' without arguments\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"will show the list of options.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		return cli_write_string(dev->cli_handler,
					(uint8_t*)"Available verbose commands.\n\n");
	} else {
		return cli_write_string(dev->cli_handler,
					(uint8_t*)"\nAvailable short commands:\n\n");
	}
}

/**
 * Help command helper function. Display the application commands tooltip.
 *
 * @param [in] dev	     - The device structure.
 * @param [in] short_command - True to display the long command prompt,
 *                             false to display the short command prompt.
 *
 * @return 0 in case of 0, negative error code otherwise.
 */
static int32_t cn0537_help_app(struct cn0537_dev *dev, bool short_command)
{
	int32_t ret;

	if (!short_command) {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" help                   - Display the help tooltip.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       " stream                 - Put the device in GO mode and stream data from the device to the terminal.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       " idle                   - Stop the stream and put the device in program mode.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       " mode_set <opt>         - Change the output mode between code (temperature compensated), PTR and raw(no temp compensation).\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          <opt> = the stream option: 'code', 'ptr' or 'raw'\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          Example: mode_set ptr - Change streaming to PTR.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" odr_set <odr>          - Set a new sample rate.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          <odr> = new sample rate in samples per second.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          Example: odr_set 0.163 - set sample rate to 1/6 SPS\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" odr_get                - Display the current sample rate.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" test                   - Do production test and print result.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" note <string>          - Print user note on the console.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"                          <string> = Text to be printed.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" file_open <name>       - Open a file on the SD card.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          <name> = name of the file to be opened.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" file_close             - Close an open file on the SD card.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
	} else {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" h               - Display the help tooltip.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       " s               - Put the device in GO mode and stream data from the device to the terminal.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       " i               - Stop the stream and put the device in program mode.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       " ms <opt>         - Change the output mode between code (temperature compensated), PTR and raw(no temp compensation).\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"                   <opt> = the stream option: 'code', 'ptr' or 'raw'\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"                   Example: ms ptr - Change streaming to PTR.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" os <odr>        - Set a new sample rate.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                   <odr> = new sample rate in samples per second.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                   Example: os 0.163 - set sample rate to 1/6 SPS\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" og              - Display the current sample rate.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" t               - Do production test and print result.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" n <string>      - Print user note on the console.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"                   <string> = Text to be printed.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" fo <name>       - Open a file on the SD card.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          <name> = name of the file to be opened.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" fc             - Close an open file on the SD card.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
	}

	return 0;
}

/**
 * Help command helper function. Display the device commands tooltip.
 *
 * @param [in] dev	     - The device structure.
 * @param [in] short_command - True to display the long command prompt,
 *                             false to display the short command prompt.
 *
 * @return 0 in case of 0, negative error code otherwise.
 */
static int32_t cn0537_help_device(struct cn0537_dev *dev, bool short_command)
{
	int32_t ret;

	if (!short_command) {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       " reg_read <addr>        - Read a register and display it's value on the terminal.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          <addr> = address of the register to be read, in hexadecimal notation.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          Example: reg_read a = read register 0xA\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" reg_write <addr> <val> - Write a device register.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          <addr> = address of the register to be written, in hexadecimal notation.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          <val> = new value to be written to the register, in hexadecimal notation.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                          Example: reg_read a 12c = write register 0xA with value 0x12C\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		return cli_write_string(dev->cli_handler,
					(uint8_t*)
					" register_dump          - Output the values of all the ADPD registers.\n");
	} else {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       " rr <addr>       - Read a register and display it's value on the terminal.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                   <addr> = address of the register to be read, in hexadecimal notation.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"                   Example: rr a = read register 0xA\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)" rw <addr> <val> - Write a device register.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                   <addr> = address of the register to be written, in hexadecimal notation.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                   <val> = new value to be written to the register, in hexadecimal notation.\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)
				       "                   Example: rw a 12c = write register 0xA with value 0x12C\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
		return cli_write_string(dev->cli_handler,
					(uint8_t*)" rd              - Output the values of all the ADPD registers.\n");
	}
}

/**
 * @brief CLI help command. Display help tooltip on the console.
 * @param dev - Pointer to the application handler.
 * @param arg - Not used in this case. Need to keep the function prototype the
 *              consistent for all CLI command functions.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_help(struct cn0537_dev *dev, uint8_t *arg)
{
	int32_t ret;

	ret = cn0537_help_prompt(dev, HELP_LONG_COMMAND);
	if(NO_OS_IS_ERR_VALUE(ret))
		return ret;

	ret = cn0537_help_app(dev, HELP_LONG_COMMAND);
	if(NO_OS_IS_ERR_VALUE(ret))
		return ret;

	ret = cn0537_help_device(dev, HELP_LONG_COMMAND);
	if(NO_OS_IS_ERR_VALUE(ret))
		return ret;

	ret = cn0537_help_prompt(dev, HELP_SHORT_COMMAND);
	if(NO_OS_IS_ERR_VALUE(ret))
		return ret;

	ret = cn0537_help_app(dev, HELP_SHORT_COMMAND);
	if(NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return cn0537_help_device(dev, HELP_SHORT_COMMAND);
}

/**
 * @brief CLI command to read and display a device register.
 * @param dev - Pointer to the application handler.
 * @param arg - The address of the register.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_reg_read(struct cn0537_dev *dev, uint8_t *arg)
{
	uint8_t reg_addr, *err_ptr, buff[50];
	int32_t ret;
	uint16_t reg_val;
	FRESULT f_ret;
	uint32_t bytes_written;

	reg_addr = strtol((char *)arg, (char **)&err_ptr, 16);
	if (err_ptr == arg) {
		sprintf((char *)buff, "Register address not found.\n");
		cli_write_string(dev->cli_handler, buff);
		return -1;
	}

	ret = adpd188_reg_read(dev->adpd188_handler, reg_addr, &reg_val);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		sprintf((char *)buff, "Register read error.\n");
		cli_write_string(dev->cli_handler, buff);
		return -1;
	}

	sprintf((char *)buff, "Register read: 0x%x = 0x%x\n", reg_addr,
		reg_val);

	ret = cli_write_string(dev->cli_handler, buff);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	if (SD_CARD && dev->file_open) {
		f_ret = f_write(&dev->curr_file, buff, strlen((char *)buff),
				(UINT *)&bytes_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"ERROR: SD card write fail.\n");
			return -1;
		}
	}

	return 0;
}

/**
 * @brief CLI command to write a device register.
 * @param dev - Pointer to the application handler.
 * @param arg - The address and new value of the register.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_reg_write(struct cn0537_dev *dev, uint8_t *arg)
{
	uint8_t reg_addr, *err_ptr, buff[50];
	uint16_t reg_val;
	FRESULT f_ret;
	int32_t ret;
	uint32_t bytes_written;

	reg_addr = strtol((char *)arg, (char **)&err_ptr, 16);
	if (err_ptr == arg) {
		sprintf((char *)buff, "Register address not found.\n");
		cli_write_string(dev->cli_handler, buff);
		return -1;
	}

	reg_val = strtol((char *)(err_ptr + 1), (char **)&arg, 16);
	if ((err_ptr + 1) == arg) {
		sprintf((char *)buff, "Register value not found.\n");
		cli_write_string(dev->cli_handler, buff);
		return -1;
	}

	sprintf((char *)buff, "Register write: 0x%x = 0x%x.\n", reg_addr,
		reg_val);
	cli_write_string(dev->cli_handler, buff);

	ret = adpd188_reg_write(dev->adpd188_handler, reg_addr, reg_val);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	if (SD_CARD && dev->file_open) {
		f_ret = f_write(&dev->curr_file, buff, strlen((char *)buff),
				(UINT *)&bytes_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"ERROR: SD card write fail.\n");
			return -1;
		}
	}

	return 0;
}

/**
 * @brief CLI command to start streaming data from the smoke detector.
 * @param dev - Pointer to the application handler.
 * @param arg - The number of data samples to display. If 0 or left unmentioned
 *              stream indefinitely.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_stream(struct cn0537_dev *dev, uint8_t *arg)
{
	uint8_t buff[100]  __attribute__ ((aligned (4)));
	uint32_t no_written;
	int32_t ret;
	FRESULT f_ret;

	dev->stream_cnt = 1;
	dev->stream_cnt_en = false;
	if ((uint32_t)arg != 1) {
		dev->stream_cnt = atoi((char *)arg);
		dev->stream_cnt_en = true;
	}
	if (!dev->stream_cnt) {
		dev->stream_cnt = 1;
		dev->stream_cnt_en = false;
	}

	if (!dev->stream_en) {
		ret = adpd188_mode_set(dev->adpd188_handler, ADPD188_NORMAL);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
	}

	cn0537_stream_update_ptr_param(dev);

	sprintf((char *)buff, "Blue PTR scaler: %.5e\nIR PTR scaler: %.5e\n",
		(double)blue_ptr_coeff, (double)ir_ptr_coeff);
	cli_write_string(dev->cli_handler, buff);

	sprintf((char *)buff,
		"Format: blue_ch ir_ch time_sec temperature\n");
	cli_write_string(dev->cli_handler, buff);
	if (SD_CARD && dev->sd_handler->spi_desc && dev->file_open) {
		sprintf((char *)buff, "Blue PTR scaler: %.5e\nIR PTR scaler: %.5e\n",
			(double)blue_ptr_coeff, (double)ir_ptr_coeff);
		f_ret = f_write(&dev->curr_file, buff, strlen((char *)buff),
				(UINT *)&no_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"File write error.\n");
			return -1;
		}

		sprintf((char *)buff,
			"Format: blue_ch ir_ch time_sec temperature\n");
		f_ret = f_write(&dev->curr_file, buff, strlen((char *)buff),
				(UINT *)&no_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"File write error.\n");
			return -1;
		}
	}

	dev->stream_en = true;
	dev->idle = 0;

	return 0;
}

/**
 * @brief CLI command to stop streaming and/or reading smoke data.
 * @param dev - Pointer to the application handler.
 * @param arg - if 1 or unmentioned stop streaming and reading data;
 *              if 0 stop streaming data to the terminal.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_idle(struct cn0537_dev *dev, uint8_t *arg)
{
	int32_t ret;
	uint8_t temp_state;

	if (((uint32_t)arg == 1) || (arg[0] == 0))
		temp_state = 1;
	else
		temp_state = atoi((char *)arg);

	if (!temp_state) {
		if (dev->idle == 1) {
			ret = adpd188_mode_set(dev->adpd188_handler,
					       ADPD188_NORMAL);
			if (NO_OS_IS_ERR_VALUE(ret))
				return -1;
			cn0537_stream_update_ptr_param(dev);
		}
		dev->idle = temp_state;
		dev->stream_cnt = 0;
		dev->stream_cnt_en = false;

		return 0;
	}

	if (dev->stream_en || !dev->idle) {
		ret = adpd188_mode_set(dev->adpd188_handler, ADPD188_PROGRAM);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
	}

	dev->stream_en = false;
	dev->idle = 1;

	return 0;
}

/**
 * @brief Set stream mode to code or PTR.
 * @param dev - Pointer to the application handler.
 * @param arg - Mode to change to: CODE or PTR.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_mode_set(struct cn0537_dev *dev, uint8_t *arg)
{
	uint8_t i = 0;
	uint8_t buff[20];
	FRESULT f_ret;
	uint32_t bytes_written;

	if (!arg) {
		cli_write_string(dev->cli_handler,
				 (uint8_t *)"ERROR invalid arguments.\n");
		return -1;
	}

	while (i != strlen((char *)arg)) {
		arg[i] = toupper(arg[i]);
		i++;
	}

	i = 0;
	while(cn0537_modes[i][0] != 0) {
		if (strcmp((char *)arg, (char *)cn0537_modes[i]) == 0)
			break;
		i++;
	}
	if (i > CN0537_RAW) {
		cli_write_string(dev->cli_handler,
				 (uint8_t *)"ERROR invalid arguments.\n");
		return -1;
	}

	dev->stream_mode = i;

	sprintf((char *)buff, "New mode is: %s\n", cn0537_modes[i]);
	cli_write_string(dev->cli_handler, buff);

	if (SD_CARD && dev->file_open) {
		f_ret = f_write(&dev->curr_file, buff, strlen((char *)buff),
				(UINT *)&bytes_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"ERROR: SD card write fail.\n");
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Read and disply all ADPD device registers.
 * @param dev - Pointer to the application handler.
 * @param arg - Not used in this case. Need to keep the function prototype the
 *              consistent for all CLI command functions.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_register_dump(struct cn0537_dev *dev, uint8_t *arg)
{
	uint8_t i;
	int32_t ret;
	uint8_t buff[50];
	uint16_t reg_val;

	cli_write_string(dev->cli_handler, (uint8_t *)"Register values:\n");

	for (i = 0; i <= ADPD188_REG_B_PD4_HIGH; i++) {
		ret = adpd188_reg_read(dev->adpd188_handler, i, &reg_val);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			sprintf((char *)buff, "Register read error.\n");
			cli_write_string(dev->cli_handler, buff);
			return -1;
		}
		sprintf((char *)buff, "\t0x%x = 0x%x\n", i, reg_val);
		cli_write_string(dev->cli_handler, buff);
	}

	return 0;
}

/**
 * @brief Calculate and display the output data rate.
 * @param dev - Pointer to the application handler.
 * @param arg - Not used in this case. Need to keep the function prototype the
 *              consistent for all CLI command functions.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_odr_get(struct cn0537_dev *dev, uint8_t *arg)
{
	int32_t ret;
	uint16_t reg_val;
	float odr;
	uint8_t buff[50];
	FRESULT f_ret;
	uint32_t bytes_written;

	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_FSAMPLE,
			       &reg_val);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	odr = 32000.0 / (reg_val * 4.0);

	sprintf((char *)buff, "Current sample rate: %.3f\n", (double)odr);
	cli_write_string(dev->cli_handler, buff);

	if (SD_CARD && dev->file_open) {
		f_ret = f_write(&dev->curr_file, buff, strlen((char *)buff),
				(UINT *)&bytes_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"ERROR: SD card write fail.\n");
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Set output data rate.
 * @param dev - Pointer to the application handler.
 * @param arg - New output data rate in hertz.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_odr_set(struct cn0537_dev *dev, uint8_t *arg)
{
	int32_t ret;
	uint8_t *err_ptr, buff[50];
	float odr;
	uint16_t reg_val;
	FRESULT f_ret;
	uint32_t bytes_written;

	odr = strtod((char *)arg, (char **)&err_ptr);
	if (err_ptr == arg)
		return -1;

	reg_val = 32000 / (odr * 4);
	ret = adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_FSAMPLE,
				reg_val);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	odr = 32000.0 / (reg_val * 4.0);
	sprintf((char *)buff, "Sample rate set: %.3f\n", (double)odr);
	cli_write_string(dev->cli_handler, buff);

	if (SD_CARD && dev->file_open) {
		f_ret = f_write(&dev->curr_file, buff, strlen((char *)buff),
				(UINT *)&bytes_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"ERROR: SD card write fail.\n");
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Implement test procedure and display PASS if the system is working and
 *        FAIL if there is a problem.
 * @param dev - Pointer to the application handler.
 * @param arg - Not used in this case. Need to keep the function prototype the
 *              consistent for all CLI command functions.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_func_test(struct cn0537_dev *dev, uint8_t *arg)
{
	int32_t ret, i = 0;
	bool data_rdy;
	uint16_t odr_temp;

	ret = adpd188_reg_read(dev->adpd188_handler, ADPD188_REG_FSAMPLE,
			       &odr_temp);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	ret = adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_FSAMPLE, 250);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	if (!dev->stream_en) {
		ret = adpd188_mode_set(dev->adpd188_handler, ADPD188_NORMAL);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -1;
	} else {
		return -1;
	}

	while (i < 20) {
		ret = cn0537_process_get_data(dev, &data_rdy);
		if (ret != 0)
			return -1;
		if (data_rdy) {
			if ((dev->blue_ptr < 6.0) || (dev->blue_ptr > 10.0))
				break;
			if ((dev->ir_ptr < 4.0) || (dev->ir_ptr > 8.0))
				break;
			i++;
		}
	}

	if (i == 20) {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"PASS!!\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
	} else {
		ret = cli_write_string(dev->cli_handler,
				       (uint8_t*)"FAIL!!\n");
		if(NO_OS_IS_ERR_VALUE(ret))
			return ret;
	}

	ret = adpd188_mode_set(dev->adpd188_handler, ADPD188_PROGRAM);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	return adpd188_reg_write(dev->adpd188_handler, ADPD188_REG_FSAMPLE,
				 odr_temp);
}

/**
 * @brief Display custom note on the terminal and print it to SD card log.
 * @param dev - Pointer to the application handler.
 * @param arg - Custom note to display.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_note(struct cn0537_dev *dev, uint8_t *arg)
{
	int32_t ret;
	FRESULT f_ret;
	uint32_t no_written;

	ret = cli_write_string(dev->cli_handler, arg);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	if (SD_CARD && dev->sd_handler->spi_desc && dev->file_open) {
		f_ret = f_write(&dev->curr_file, arg, strlen((char *)arg),
				(UINT *)&no_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"File write error.\n");
			return -1;
		}
		f_ret = f_write(&dev->curr_file, "\n", strlen("\n"),
				(UINT *)&no_written);
		if (f_ret != FR_OK) {
			cli_write_string(dev->cli_handler,
					 (uint8_t *)"File write error.\n");
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Open a file on the SD card and start logging. Only one file may be
 *        open at a time.
 * @param dev - Pointer to the application handler.
 * @param arg - Name of the file to be opened.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_file_open(struct cn0537_dev *dev, uint8_t *arg)
{
	FRESULT f_ret;
	uint8_t name_buff[50];

	if (!SD_CARD)
		return 0;

	if (dev->file_open) {
		cli_write_string(dev->cli_handler,
				 (uint8_t *)"A file is already open.\n");
		return 0;
	}

	if (strlen((char *)arg) > 8) {
		cli_write_string(dev->cli_handler,
				 (uint8_t *)"ERROR: File name too long.\n");
		return -1;
	}

	if (!dev->sd_handler->spi_desc) {
		cli_write_string(dev->cli_handler,
				 (uint8_t *)"ERROR: SD card not found.\n");
		return -1;
	}

	sprintf((char *)name_buff, "0:");
	strcat((char *)name_buff, (char *)arg);
	strcat((char *)name_buff, ".txt");
	f_ret = f_open(&dev->curr_file, (char *)name_buff,
		       FA_WRITE | FA_OPEN_APPEND);
	if (f_ret != FR_OK) {
		cli_write_string(dev->cli_handler,
				 (uint8_t *)"ERROR: File not open.\n");
		return -1;
	}
	dev->file_open = true;

	cli_write_string(dev->cli_handler, (uint8_t *)"File named ");
	cli_write_string(dev->cli_handler, name_buff);
	cli_write_string(dev->cli_handler, (uint8_t *)" open.\n");

	return 0;
}

/**
 * @brief Close the opened file.
 * @param dev - Pointer to the application handler.
 * @param arg - Not used in this case. Need to keep the function prototype the
 *              consistent for all CLI command functions.
 * @return 0 in case of 0, -1 otherwise.
 */
int32_t cn0537_file_close(struct cn0537_dev *dev, uint8_t *arg)
{
	FRESULT f_ret;

	if (!SD_CARD)
		return 0;

	if (!dev->file_open) {
		cli_write_string(dev->cli_handler,
				 (uint8_t *)"File not open.\n");
		return -1;
	}

	f_ret = f_close(&dev->curr_file);
	if (f_ret != FR_OK) {
		cli_write_string(dev->cli_handler,
				 (uint8_t *)"ERROR: File could not close.\n");
		return -1;
	}
	dev->file_open = false;

	cli_write_string(dev->cli_handler, (uint8_t *)"File closed.\n");

	return 0;
}
