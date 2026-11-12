/***************************************************************************//**
 *   @file   main.c
 *   @brief  AD7291 voltage monitor example for Raspberry Pi (Linux platform).
 *           Reads and prints all 8 channel voltages in a loop.
 * 	 @author	 Melissa Makonga (melissa.makonga@analog.com)
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include "ad7291.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "linux_i2c.h"

#define AD7291_I2C_ADDRESS	0x2A
#define AD7291_I2C_BUS		1

static const char *ch_labels[] = {
	"VDD1V8 ",
	"VDD3V0 ",
	"VDD3V3 ",
	"VDD4V5 ",
	"VDD_AMP",
	"VINPUT ",
	"IMON   ",
	"VTUNE  ",
};

/* Resistor-divider scale factors from CN0566 schematic (x1000 for integer math).
 * Actual voltage = ADC reading * scale factor. */
static const int32_t ch_scale_x1000[] = {
	2000,	/* CH0: 1 + 10k/10k = 2.0 */
	2000,	/* CH1: 1 + 10k/10k = 2.0 */
	2000,	/* CH2: 1 + 10k/10k = 2.0 */
	4010,	/* CH3: 1 + 30.1k/10k = 4.01 */
	7980,	/* CH4: 1 + 69.8k/10k = 7.98 */
	4010,	/* CH5: 1 + 30.1k/10k = 4.01 */
	1000,	/* CH6: 1.0 (LTC4217 IMON: 50uA/A * 20k = 1V/A) */
	7980,	/* CH7: 1 + 69.8k/10k = 7.98 */
};

int main(void)
{
	struct ad7291_desc *dev;
	struct linux_i2c_init_param linux_i2c_extra = {
		.device_id = AD7291_I2C_BUS,
	};
	struct ad7291_init_param init_param = {
		.i2c_init = {
			.device_id = AD7291_I2C_BUS,
			.max_speed_hz = 400000,
			.slave_address = AD7291_I2C_ADDRESS,
			.platform_ops = &linux_i2c_ops,
			.extra = &linux_i2c_extra,
		},
		.vref_mv = 0,
	};
	int32_t millivolts;
	int32_t scaled_mv;
	uint8_t ch;
	int ret;

	ret = ad7291_init(&dev, &init_param);
	if (ret) {
		printf("AD7291 init failed: %d\n", ret);
		return ret;
	}

	printf("AD7291 initialized at 0x%02X on /dev/i2c-%d\n",
	       AD7291_I2C_ADDRESS, AD7291_I2C_BUS);

	while (1) {
		for (ch = 0; ch < AD7291_NUM_CHANNELS; ch++) {
			ret = ad7291_read_channel_voltage(dev, ch, &millivolts);
			if (ret) {
				printf("CH%d %-7s read error: %d\n",
				       ch, ch_labels[ch], ret);
				continue;
			}
			scaled_mv = millivolts * ch_scale_x1000[ch] / 1000;
			printf("CH%d %s: %d.%03d V\n", ch, ch_labels[ch],
			       (int)(scaled_mv / 1000),
			       (int)(scaled_mv % 1000));
		}
		printf("---\n");
		no_os_mdelay(1000);
	}

	ad7291_remove(dev);

	return 0;
}
