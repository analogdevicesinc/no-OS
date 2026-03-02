/***************************************************************************//**
 *   @file   ad4692.h
 *   @brief  Header file for ad4692 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
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
*******************************************************************************/
#ifndef _AD4692_H_
#define _AD4692_H_

#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_pwm.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"

#define AD4692_FRAME_SIZE			6

#define AD4692_CONV_START_MASK			NO_OS_BIT(0)
#define AD4692_CHANNEL_MASK(n)			NO_OS_BIT(n)
#define AD4692_MAX_CHANNELS			16

#define AD4692_DEFAULT_OSC_FREQ			1000000
#define AD4692_CNV_DUTY_CYCLE_NS		210

#define AD4692_VREF_MIN				2400000
#define AD4692_VREF_2P75			2750000
#define AD4692_VREF_3P25			3250000
#define AD4692_VREF_3P75			3750000
#define AD4692_VREF_4P50			4500000
#define AD4692_VREF_MAX				5250000

#define AD4692_REF_VOLTAGE_2P75			0x00
#define AD4692_REF_VOLTAGE_3P25			0x04
#define AD4692_REF_VOLTAGE_3P75			0x08
#define AD4692_REF_VOLTAGE_4P50			0x0C
#define AD4692_REF_VOLTAGE_5			0x10

#define AD4692_INTERFACE_CONFIG_A_REG		0x00
#define AD4692_INTERFACE_STATUS_A_REG		0x11
#define AD4692_REG_ACCESS_ERR_MASK		NO_OS_BIT(1)
#define AD4692_PRODUCT_ID_LSB_REG		0x04
#define AD4692_PRODUCTID_HIGH_REG		0x05
#define AD4692_CLEAR_IA_STATUS			0x8F

#define AD4692_STATUS_REG			0x14
#define AD4692_CLAMP_STATUS1_REG		0x1A
#define AD4692_CLAMP_STATUS2_REG		0x1B
#define AD4692_DEVICE_SETUP_REG			0x20
#define AD4692_REF_CTRL_REG			0x21
#define AD4692_SEQUENCER_CONTROL_REG		0x22
#define AD4692_OSC_FREQ_REG			0x23
#define AD4692_STD_SEQ_CONFIG_REG		0x25
#define AD4692_SPARE_CONTROL_REG		0x2A
#define AD4692_CHANNEL_CONFIG_REG(n)		(0x30 + (n))

#define AD4692_OSC_EN_REG			0x180
#define AD4692_STATE_RESET_REG			0x181
#define AD4692_ADC_SETUP_REG			0x182
#define AD4692_ACC_MASK1_REG			0x184
#define AD4692_ACC_MASK2_REG			0x185
#define AD4692_ACC_COUNT_LIMIT_REG(n)		(0x186 + (n))
#define AD4692_GPIO_MODE1_REG			0x196
#define AD4692_GPIO_MODE2_REG			0x197
#define AD4692_GPIO_READ_REG			0x1A0
#define AD4692_ACC_STATUS_FULL1_REG		0x1B0
#define AD4692_ACC_STATUS_FULL2_REG		0x1B1
#define AD4692_ACC_STATUS_OVERRUN1_REG		0x1B2
#define AD4692_ACC_STATUS_OVERRUN2_REG		0x1B3
#define AD4692_ACC_STATUS_SAT1_REG		0x1B4
#define AD4692_ACC_STATUS_SAT2_REG		0x1BE
#define AD4692_ACC_SAT_OVR_REG(n)		(0x1C0 + (n))
#define AD4692_AVG_IN_REG(n)			(0x201 + 2 * (n))
#define AD4692_AVG_STS_IN_REG(n)		(0x222 + 3 * (n))
#define AD4692_ACC_IN_REG(n)			(0x252 + 3 * (n))
#define AD4692_ACC_STS_DATA_REG(n)		(0x283 + 4 * (n))

#define AD4692_RW_ADDR_MASK			NO_OS_BIT(7)
#define AD4692_MSB_MASK(n)			((n) >> 8)
#define AD4692_LSB_MASK(n)			((n) & 0xFF)

#define AD4692_SW_RST_MASK			(NO_OS_BIT(7) | NO_OS_BIT(0))

#define AD4692_MANUAL_MODE_MASK			NO_OS_BIT(2)
#define AD4692_MANUAL_DATA_MASK			NO_OS_GENMASK(23, 8)
#define AD4692_MODE_MASK			NO_OS_GENMASK(1, 0)

#define AD4692_16B_MASK				NO_OS_GENMASK(15, 0)
#define AD4692_24B_MASK				NO_OS_GENMASK(23, 0)

#define AD4692_STANDARD_SEQ_MODE		0x80
#define AD4692_ADV_SEQ_MODE_MASK		NO_OS_GENMASK(6, 0)

#define AD4692_OSC_FREQ_MASK			NO_OS_GENMASK(3, 0)
#define AD4692_REF_CTRL_MASK			NO_OS_GENMASK(4, 2)

#define AD4692_GPIO0_MASK			NO_OS_GENMASK(3, 0)
#define AD4692_GPIO1_MASK			NO_OS_GENMASK(7, 4)
#define AD4692_GPIO2_MASK			NO_OS_GENMASK(3, 0)
#define AD4692_GPIO3_MASK			NO_OS_GENMASK(7, 4)
#define AD4692_GP_READ_MASK(n)			NO_OS_BIT(n)

#define AD4692_SEQ_ALL_CHANNELS_OFF		0x00
#define AD4692_STATE_RESET_ALL			0x01

#define AD4692_MAX_CONV_PERIOD_US		800

#define AD4692_DEVICE_MANUAL			0x14

#define AD4692_IN_VAL_MASK(n)			NO_OS_GENMASK(23, 24 - (n))

#define AD4692_NOOP				0x00
#define AD4692_EXIT_COMMAND			(0x0A << 3)
#define AD4692_TEMPERATURE_SENSOR		(0x0F << 3)
#define AD4692_IN_COMMAND(n)			((0x10 + (n)) << 3)

enum ad4692_ref_ctrl_sel {
	AD4692_VREF2P5,
	AD4692_VREF3P0,
	AD4692_VREF3P3,
	AD4692_VREF4P096,
	AD4692_VREF5P0,
};

enum ad4692_int_osc_sel {
	AD4692_OSC_1MHZ,
	AD4692_OSC_500KHZ,
	AD4692_OSC_400KHZ,
	AD4692_OSC_250KHZ,
	AD4692_OSC_200KHZ,
	AD4692_OSC_167KHZ,
	AD4692_OSC_133KHZ,
	AD4692_OSC_125KHZ,
	AD4692_OSC_100KHZ,
	AD4692_OSC_50KHZ,
	AD4692_OSC_25KHZ,
	AD4692_OSC_12P5KHZ,
	AD4692_OSC_10KHZ,
	AD4692_OSC_5KHZ,
	AD4692_OSC_2P5KHZ,
	AD4692_OSC_1P25KHZ,
};

enum ad4692_spi_mode {
	AD4692_CNV_CLOCK,
	AD4692_CNV_BURST,
	AD4692_AUTONOMOUS,
	AD4692_SPI_BURST,
	AD4692_MANUAL_MODE,
};

enum ad4692_gpio_sel {
	AD4692_GPIO_HIGHZ,
	AD4692_GPIO_OUTPUT_LOW,
	AD4692_GPIO_OUTPUT_HIGH,
	AD4692_GPIO_INPUT,
	AD4692_GPIO_OUTPUT_ADC_BUSY,
	AD4692_GPIO_OUTPUT_SEQ_DONEb,
	AD4692_GPIO_OUTPUT_DATA_READYb,
	AD4692_GPIO_OUTPUT_ACC_OVF_ERR,
	AD4692_GPIO_OUTPUT_ACC_SAT_ERR,
};

enum ad4692_gpio_num {
	AD4692_GPIO0,
	AD4692_GPIO1,
	AD4692_GPIO2,
	AD4692_GPIO3,
};

enum ad4692_type {
	ID_AD4692,
	ID_AD4691,
	ID_AD4694,
	ID_AD4693,
};

extern const int ad4692_int_osc_val[16];

struct ad4692_init_param {
	struct no_os_spi_init_param	*comm_param;

	struct no_os_gpio_init_param	*reset_param;

	struct no_os_gpio_init_param	*gpio0_param;
	struct no_os_gpio_init_param	*gpio1_param;
	struct no_os_gpio_init_param	*gpio2_param;
	struct no_os_gpio_init_param	*gpio3_param;

	struct no_os_pwm_init_param	*conv_param;

	uint32_t			vref;

	enum ad4692_spi_mode		mode;
	enum ad4692_type		id;
};

struct ad4692_desc {
	struct no_os_spi_desc		*comm_desc;

	struct no_os_gpio_desc		*reset_desc;

	struct no_os_gpio_desc		*gpio0_desc;
	struct no_os_gpio_desc		*gpio1_desc;
	struct no_os_gpio_desc		*gpio2_desc;
	struct no_os_gpio_desc		*gpio3_desc;

	struct no_os_pwm_desc		*conv_desc;

	enum ad4692_spi_mode		mode;
	uint8_t				buff[AD4692_FRAME_SIZE + 1];

	int				max_rate;
	uint32_t			vref;
	uint8_t				num_channels;
	enum ad4692_type		id;
};

int ad4692_cnv_burst_period_ns(struct ad4692_desc *desc, int n_active,
			       uint32_t *period_ns);

int ad4692_send_command(struct ad4692_desc *desc, uint32_t command,
			uint32_t *val);

int ad4692_reg_read(struct ad4692_desc *desc, uint32_t reg, uint32_t *val);

int ad4692_reg_write(struct ad4692_desc *desc, uint32_t reg, uint32_t val);

int ad4692_reg_update(struct ad4692_desc *desc, uint32_t reg, uint32_t mask,
		      uint32_t val);

int ad4692_std_seq_ch(struct ad4692_desc *desc, uint16_t ch_mask, bool std,
		      uint8_t len);

int ad4692_sampling_enable(struct ad4692_desc *desc, bool enable);

int ad4692_get_ch(struct ad4692_desc *desc, uint8_t ch, uint32_t *val);

int ad4692_get_temp(struct ad4692_desc *desc, uint32_t *val);

int ad4692_set_osc(struct ad4692_desc *desc, enum ad4692_int_osc_sel osc);

int ad4692_get_osc(struct ad4692_desc *desc, enum ad4692_int_osc_sel *osc);

int ad4692_set_ref(struct ad4692_desc *desc, enum ad4692_ref_ctrl_sel ref);

int ad4692_get_ref(struct ad4692_desc *desc, enum ad4692_ref_ctrl_sel *ref);

int ad4692_gpio_set(struct ad4692_desc *desc, enum ad4692_gpio_num index,
		    enum ad4692_gpio_sel mode);

int ad4692_gpio_get_value(struct ad4692_desc *desc, enum ad4692_gpio_num index,
			  enum no_os_gpio_values *value);

int ad4692_hardware_reset(struct ad4692_desc *desc);

int ad4692_software_reset(struct ad4692_desc *desc);

int ad4692_init(struct ad4692_desc **desc,
		struct ad4692_init_param *init_param);

int ad4692_remove(struct ad4692_desc *desc);

#endif /* _AD4692_H_ */
