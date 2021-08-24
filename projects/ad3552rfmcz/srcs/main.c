/***************************************************************************//**
 *   @file   ad3552r/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "app_config.h"
#include "parameters.h"
#include "error.h"
#include "print_log.h"
#include "iio_app.h"
#include "iio_timer_trigger.h"
#include "iio_ad3552r.h"
#include "ad3552r.h"
#include "spi.h"
#include "gpio.h"
#include "delay.h"
#include "util.h"

#ifdef XILINX_PLATFORM

#include <xparameters.h>
#include <xil_cache.h>
#include <xil_cache.h>

#include "spi_extra.h"
#include "gpio_extra.h"
#include "irq_extra.h"
#include "timer_extra.h"
#endif

#ifdef LINUX_PLATFORM
#include "linux_spi.h"
#include "linux_gpio.h"
#endif

#include <stdint.h>

#include <math.h>


static uint8_t gpio_default_prop[][2] = {
		[GPIO_RESET_N] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_LDAC_N] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_SPI_QPI] = {GPIO_OUT, GPIO_LOW},
		[GPIO_ALERT_N] = {GPIO_IN, 0},
		[GPIO_SYNC_EVENTS] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_6] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_7] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_8] = {GPIO_OUT, GPIO_HIGH},
};

#ifdef XILINX_PLATFORM
const struct gpio_platform_ops	*gpio_ops = &xil_gpio_platform_ops;
#define SPI_OPS 	(&xil_spi_reg_ops_pl)
#endif
#ifdef LINUX_PLATFORM
const struct gpio_platform_ops	*gpio_ops = &linux_gpio_platform_ops;
#define SPI_OPS		(&linux_spi_platform_ops)
#endif

struct ad3552r_init_param default_ad3552r_param = {
	.spi_param = {
		.device_id = SPI_DEVICE_ID,
		.chip_select = 0,
		.mode = SPI_MODE_3,
		.bit_order = SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = NULL
	},
	//.timer_id = TIMER_DEVICE_ID,
	//.timer_intr_nb = TIMER_IRPT_INTR
};

struct gpio_desc *gpios[TOTAL_GPIOS];
uint32_t tout;
struct ad3552r_desc *dac;


#define COUNTS_STREAM 3
#define PRODUCT_ID	0x4008
#define CHIP_GRADE	0x3
#define SPI_REVISION	0x83
#define VENDOR		0x0456
#define MAGIC_NUMBER	0x3C
#define MAGIC_NUMBER2	0x96
#define TIMEOUT 5000
#define MAX_NB_SAMPLES 1000

void set_timeout(uint32_t timeout) {
	tout = timeout;
}

int is_timeout()
{
	for (int i = 0; i < 0xFFF; i++)
		;
	tout--;

	return !tout;
}
void hardware_reset()
{
	gpio_set_value(gpios[GPIO_RESET_N], GPIO_LOW);
	mdelay(100);
	gpio_set_value(gpios[GPIO_RESET_N], GPIO_HIGH);
	mdelay(1000);
}

int32_t init_gpios(struct gpio_desc **gpios)
{
	uint32_t i;
	int32_t	 ret;
	void *default_platform_param;

#ifdef XILINX_PLATFORM
	struct xil_gpio_init_param platorm_gpio_xil_param = {
			.device_id = GPIO_DEVICE_ID,
			.type = GPIO_PS
	};
	default_platform_param = &platorm_gpio_xil_param;
#endif
#ifdef LINUX_PLATFORM
	default_platform_param = NULL;
#endif

	struct gpio_init_param default_gpio_param = {
			.platform_ops = gpio_ops,
			.extra = default_platform_param
	};

	for (i = 0; i < TOTAL_GPIOS; i++) {
		if (i == GPIO_LDAC_N)
			continue;
		default_gpio_param.number = GPIO_OFFSET + i;
		ret = gpio_get(&gpios[i], &default_gpio_param);
		PRINT_AND_RET_ON_ERR(ret, "gpio_get failed");
		if (gpio_default_prop[i][0] == GPIO_IN)
			ret = gpio_direction_input(gpios[i]);
		else
			ret = gpio_direction_output(gpios[i],
						    gpio_default_prop[i][1]);

		PRINT_AND_RET_ON_ERR(ret, "gpio_direction_output failed");
	}

	hardware_reset();
	return ret;
}
#ifdef LINUX_PLATFORM
#include <fcntl.h>
#include <unistd.h>
void remove_gpios() {
	char path[100];
	int fd;
	int nb;
	int i;
	int len;
	for (i = 0; i < TOTAL_GPIOS; i++) {
		nb = GPIO_OFFSET + i;
		fd = open("/sys/class/gpio/unexport", O_WRONLY);
		len = sprintf(path, "%d", nb);
		write(fd, path, len);
		close(fd);
	}
}
#endif

int32_t test_communication(struct ad3552r_desc *dac)
{
	int32_t ret;
	uint32_t val;

	val = MAGIC_NUMBER;
	ret = ad3552r_write_reg(dac, AD3552R_REG_ADDR_SCRATCH_PAD, val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_SCRATCH_PAD we failed");
	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_SCRATCH_PAD, &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_VENDOR rd failed");
	if (val != MAGIC_NUMBER)
		return -1;

	return 0;
}

int32_t check_status(struct ad3552r_desc *dac)
{
	uint32_t status;
	int32_t ret;

	ret = ad3552r_get_status(dac, &status, 0);
	if (IS_ERR_VALUE(ret))
		return ret;
	ret = ad3552r_get_status(dac, &status, 1);
	if (IS_ERR_VALUE(ret))
		return ret;

	ret = ad3552r_get_status(dac, &status, 0);
	if (IS_ERR_VALUE(ret))
		return ret;

	return SUCCESS;
}

int32_t test_sw_reset(struct ad3552r_desc *dac)
{
	int32_t ret;
	uint32_t val;

	val = MAGIC_NUMBER;
	ret = ad3552r_write_reg(dac, AD3552R_REG_ADDR_SCRATCH_PAD, val);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_write_reg  failed");

	ret = ad3552r_software_reset(dac);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_software_reset failed");

	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_SCRATCH_PAD, &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_VENDOR rd failed");
	if (val != 0)
		return -1;

	return 0;
}

int32_t test_id(struct ad3552r_desc *dac)
{
	int32_t ret;
	uint32_t val;
	uint32_t val2;

	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_VENDOR_L, &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_VENDOR_L failed");
	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_VENDOR_H, &val2);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_VENDOR_H failed");
	val = (val2 << 8) | val;
	printf("Vendor: 0x%x\n", (int)val);
	if (val != VENDOR)
		return -1;

	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_CHIP_TYPE, &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_CHIP_TYPE failed");
	printf("Chip type: 0x%x\n", (int)val);
	if (val != 0x4)
		return -1;

	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_PRODUCT_ID_L, &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_PRODUCT_ID_L failed");
	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_PRODUCT_ID_H, &val2);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_PRODUCT_ID_H failed");
	val = (val2 << 8) | val;
	printf("Prod id: 0x%x\n", (int)val);
	if (val != PRODUCT_ID)
		return -1;

	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_CHIP_GRADE, &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_CHIP_GRADE failed");
	printf("Chip grade: 0x%x\n", (int)val);
	if (val != CHIP_GRADE)
		return -1;

	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_SPI_REVISION, &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_SPI_REVISION failed");
	printf("SPI Revision: 0x%x\n", (int)val);
	if (val != SPI_REVISION)
		return -1;

	return 0;
}

int32_t test_regmap_multiple_transf(struct ad3552r_desc *dac)
{
	int32_t ret;
	uint16_t samples[6];
	struct ad3552_transfer_config spi_cfg = {0};

	struct ad3552_transfer_data data = {
		.addr = AD3552R_REG_ADDR_CH_DAC_16B(1) + 1,
		.data = (uint8_t *)samples,
		.len = sizeof(uint16_t) * 2,
		.spi_cfg = &spi_cfg
	};

	samples[0] = MAGIC_NUMBER;
	samples[1] = MAGIC_NUMBER2;
	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer failed");

	data.is_read = 1;
	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer failed");

	if (!(samples[0] == MAGIC_NUMBER && samples[1] == MAGIC_NUMBER2))
		return FAILURE;

	return 0;
}

int32_t test_regmap_stream(struct ad3552r_desc *dac)
{
	int32_t ret;
	uint16_t samples[COUNTS_STREAM * 2];

	for (int i = 0; i < COUNTS_STREAM; i++)
	{
		samples[i * 2] = MAGIC_NUMBER + i;
		samples[i * 2 + 1] = MAGIC_NUMBER2 + i;
	}

	uint32_t stream = 2 * sizeof(uint16_t);
	struct ad3552_transfer_config spi_cfg = {
			.stream_mode_length = stream,
			.stream_length_keep_value = 1
	};
	struct ad3552_transfer_data data = {
		.addr = AD3552R_REG_ADDR_CH_DAC_16B(1) + 1,
		.data = (uint8_t *)samples,
		.len = stream * COUNTS_STREAM,
		.spi_cfg = &spi_cfg,
	};

	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer failed");

	data.is_read = 1;
	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer failed");
	if (!(samples[0] == MAGIC_NUMBER + COUNTS_STREAM - 1 &&
		samples[1] == MAGIC_NUMBER2 + COUNTS_STREAM - 1 &&
		samples[2] == MAGIC_NUMBER + COUNTS_STREAM - 1 &&
		samples[3] == MAGIC_NUMBER2 + COUNTS_STREAM - 1))
		return FAILURE;

	return 0;
}

typedef struct {
	uint8_t *buf;
	uint32_t i;
	int8_t i_bit;
	int8_t is_clean;
} buff_it_t;

void add_new_sample(buff_it_t *it, uint32_t sample, uint32_t store)
{
	short bit_left;
	short bit_right;
	short bit_cnt;
	short bits_remaining;
	uint32_t mask;
	uint32_t tmp;

	while (store) {
		if (!it->is_clean) {
			it->buf[it->i] = 0;
			it->is_clean = 1;
		}
		bits_remaining = 8 - it->i_bit;
		bit_left = store;
		if (bit_left > bits_remaining)
			bit_cnt = bits_remaining;
		else
			bit_cnt = bit_left;
		bit_right = bit_left - bit_cnt;
		mask = GENMASK(bit_left - 1, bit_right);
		tmp = sample & mask;
		tmp >>= bit_right;
		it->buf[it->i] |= tmp << (8 - bit_cnt - it->i_bit);
		it->i_bit += bit_cnt;
		store -= bit_cnt;
		if (it->i_bit == 8) {
			it->i++;
			it->i_bit = 0;
			it->is_clean = 0;
		}
	}
	
}


void test_add_sample() {
	int ok = 1;
	while (ok) {
		uint32_t s1;
		uint32_t s2;
		uint8_t buff[100];
		buff_it_t it = {.buf = buff};

		for (int i = 1; i < 50; i++) {
			s1 = i;
			s2 = 50 - i;
			add_new_sample(&it, s1, 12);
			add_new_sample(&it, s2, 12);
		}

	}
}

void fill_with_sine(uint8_t *buff, uint32_t samples, uint32_t nb_ch,
			uint32_t real, uint32_t store)
{
	float t;
	uint32_t tmp;
	uint32_t mask;
	buff_it_t it = {.buf = buff};

	t = 0;
	for (uint32_t i = 0; i < samples * nb_ch; i += nb_ch)
	{
		uint32_t val;
		uint32_t val2;
		val = (sin(2 * 3.14 * t) + 1.0) / 2 * UINT32_MAX;
		val2 = (sin(2 * 2 * 3.14 * t) + 1.0) / 2 * UINT32_MAX;

		tmp = 32 - store;
		mask = GENMASK(32 - 1, tmp);
		val = (val & mask) >> tmp;
		val2 = (val2 & mask) >> tmp;

		tmp = store - real;
		mask = GENMASK(store - 1, tmp);
		val &= mask;
		val2 &= mask;

		add_new_sample(&it, val, store);
		if (nb_ch == 2)
			add_new_sample(&it, val2, store);

		t += 1.0 / (samples - 1) * nb_ch;
	}
}

int32_t test_sine_single(struct ad3552r_desc *dac) {
	int32_t ret;
	pr_info("test_sine_single each ch\n");
	int ok = 1;
	set_timeout(TIMEOUT / 5);
	uint16_t sin_lut[MAX_NB_SAMPLES * 2];
	fill_with_sine((uint8_t *)sin_lut, MAX_NB_SAMPLES, 2, 16, 16);
	while(!is_timeout() && ok) {
		for (int i = 0; i < MAX_NB_SAMPLES; i++) {
			uint16_t tmp = bswap_constant_16(sin_lut[i * 2]);
			ret = ad3552r_set_ch_value(dac, AD3552R_CH_CODE, 0, tmp);
			PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_channel_value1 failed");
			tmp = bswap_constant_16(sin_lut[i * 2 + 1]);
			ret = ad3552r_set_ch_value(dac, AD3552R_CH_CODE, 1, tmp);
			PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_channel_value2 failed");

		}

	}

	pr_info("test_sine_single mask both ch\n");
	set_timeout(TIMEOUT / 5);
	while(!is_timeout() && ok) {
		for (int i = 0; i < MAX_NB_SAMPLES; i+= 2) {
			uint16_t tmp[] = {bswap_constant_16(sin_lut[i]),
					bswap_constant_16(sin_lut[i + 1])};
			ret = ad3552r_set_ch_value(dac, AD3552R_CH_CODE, 0, tmp[0]);
			ret = ad3552r_set_ch_value(dac, AD3552R_CH_CODE, 1, tmp[1]);
			PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_channel_value1 failed");
		}

	}

	return SUCCESS;
}

int32_t test_sine_stream(struct ad3552r_desc *dac) {
	int32_t ret;
	uint16_t sin_lut[MAX_NB_SAMPLES * 2];
	struct ad3552_transfer_config spi_cfg = {0};

	for (int i = 1; i <= 2; i++) {
	//for (int i = 2; i <= 2; i++) {
		if (i == 1)
			ad3552r_prepare_write(dac, 1);
		else
			ad3552r_prepare_write(dac, 3);

		pr_info("test_sine_stream. ch: %d\n", i);
		fill_with_sine((uint8_t *)sin_lut, MAX_NB_SAMPLES, i, 16, 16);

		spi_cfg.stream_mode_length = sizeof(uint16_t) * i;
		spi_cfg.stream_length_keep_value = 1;
		struct ad3552_transfer_data data = {
			.addr = AD3552R_REG_ADDR_CH_DAC_16B(i - 1) + 1,
			.data = (uint8_t *)sin_lut,
			.len = MAX_NB_SAMPLES * sizeof(*sin_lut) * i,
			.spi_cfg = &spi_cfg
		};

		set_timeout(TIMEOUT);
		int ok = 1;
		while (!is_timeout() && ok == 1){
		//while (ok) {
			ret = ad3552r_transfer(dac, &data);
			PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer failed");
		}
	}


	return SUCCESS;
}

int32_t print_chs(struct ad3552r_desc *dac, uint32_t real, uint32_t store)
{
	int32_t ret;
	uint8_t *sin_lut;

	sin_lut = malloc(MAX_NB_SAMPLES * AD3552R_NUM_CH * (store / 8));

	for (int i = 1 ; i <= 3; ++i) {
		pr_info("test_write_dev with 0x%x mask\n", i);

		ad3552r_prepare_write(dac, i);
		fill_with_sine(sin_lut, MAX_NB_SAMPLES, hweight8(i), real, store);


		set_timeout(TIMEOUT / 4);
		int ok = 1;
		while (!is_timeout() && ok ){
			ret = ad3552r_write_dev(dac, (uint8_t*)sin_lut,
						MAX_NB_SAMPLES);
			PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer failed");
		}
	}

	free(sin_lut);

	return SUCCESS;
}

int32_t test_write_dev(struct ad3552r_desc *dac)
{
	int32_t ret;

	ret = ad3552r_set_dev_value(dac, AD3552R_PRECISION_MODE_ENABLED, 0);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");
	ret = ad3552r_set_dev_value(dac, AD3552R_UPDATE_MODE, AD3552R_UPDATE_DAC);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	return print_chs(dac, AD3552R_REAL_BITS_FAST_MODE, AD3552R_STORAGE_BITS_FAST_MODE);
}

int32_t test_write_dev_precision(struct ad3552r_desc *dac)
{
	int32_t ret;

	ret = ad3552r_set_dev_value(dac, AD3552R_PRECISION_MODE_ENABLED, 1);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");
	ret = ad3552r_set_dev_value(dac, AD3552R_UPDATE_MODE, AD3552R_UPDATE_DAC);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	return print_chs(dac, AD3552R_REAL_BITS_PREC_MODE, AD3552R_STORAGE_BITS_PREC_MODE);
}

int32_t test_write_dev_input(struct ad3552r_desc *dac)
{
	int32_t ret;

	ret = ad3552r_set_dev_value(dac, AD3552R_PRECISION_MODE_ENABLED, 0);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");
	ret = ad3552r_set_dev_value(dac, AD3552R_UPDATE_MODE, AD3552R_UPDATE_INPUT);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	return print_chs(dac, AD3552R_REAL_BITS_FAST_MODE, AD3552R_STORAGE_BITS_FAST_MODE);
}

int32_t test_write_dev_precision_input(struct ad3552r_desc *dac)
{
	int32_t ret;

	ret = ad3552r_set_dev_value(dac, AD3552R_PRECISION_MODE_ENABLED, 1);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");
	ret = ad3552r_set_dev_value(dac, AD3552R_UPDATE_MODE, AD3552R_UPDATE_INPUT);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	return print_chs(dac, AD3552R_REAL_BITS_PREC_MODE, AD3552R_STORAGE_BITS_PREC_MODE);
}

int32_t test_crc_single(struct ad3552r_desc *dac) {
	int32_t ret;

	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, 1);
	if (IS_ERR_VALUE(ret))
		return ret;

	/* Single instrction test */
	ret = test_communication(dac);
	if (IS_ERR_VALUE(ret))
		return ret;

	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, 0);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	return SUCCESS;
}

int32_t test_crc_stream_reg_different_values(struct ad3552r_desc *dac)
{
	int32_t ret;
	uint8_t buff[AD3552R_REG_ADDR_MAX];
	uint8_t addr = 0x44;
	uint32_t len = addr - 0x32;
	struct ad3552_transfer_data data = {
			.addr = addr,
			.data = (uint8_t *)buff,
			.len = len
	};

	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, 1);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	data.is_read = 1;
	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer read failed");

	for (uint32_t i = 0; i < len; i++)
		buff[i] = i;

	/* Write dummy data */
	data.is_read = 0;
	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer read failed");

	/* Read written data */
	data.is_read = 1;
	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_transfer read failed");
	for (uint32_t i = 0; i < len; i++)
		if (buff[i] != i)
			return FAILURE;

	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, 0);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	return SUCCESS;
}

int32_t test_crc_stream(struct ad3552r_desc *dac)
{
	int32_t ret;
	uint16_t val;
	struct ad3552_transfer_data data = {
			.addr = AD3552R_REG_ADDR_PRODUCT_ID_H,
			.data = (uint8_t *)&val,
			.len = 2
	};

	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, 1);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	data.is_read = 1;
	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_PRODUCT_ID failed");
	val = ((val & 0xFF) << 8) | ((val & 0xFF00) >> 8);
	if (val != PRODUCT_ID)
		return -1;

	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, true);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	ret = ad3552r_transfer(dac, &data);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_PRODUCT_ID failed");
	val = ((val & 0xFF) << 8) | ((val & 0xFF00) >> 8);
	if (val != PRODUCT_ID)
		return -1;


	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, false);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	return 0;
}

int32_t test_crc_multiple_bytes(struct ad3552r_desc *dac)
{
	int32_t ret;
	uint32_t val;


	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, 1);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	/* This is not working */
	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_CH_DAC_16B(0), &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_CH_DAC_16B failed");
	ret = ad3552r_write_reg(dac, AD3552R_REG_ADDR_CH_DAC_16B(0), PRODUCT_ID);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_CH_DAC_16B failed");
	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_CH_DAC_16B(0), &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_CH_DAC_16B failed");
	if (val != PRODUCT_ID)
		return -1;

	ret = ad3552r_write_reg(dac, AD3552R_REG_ADDR_CH_DAC_24B(0), MAGIC_NUMBER);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_CH_DAC_24B failed");
	ret = ad3552r_read_reg(dac, AD3552R_REG_ADDR_CH_DAC_24B(0), &val);
	PRINT_AND_RET_ON_ERR(ret, "AD3552R_REG_ADDR_CH_DAC_24B failed");
	if (val != MAGIC_NUMBER)
		return -1;

	ret = ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, 0);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_set_dev_value failed");

	return SUCCESS;
}

int32_t test_timer(struct ad3552r_desc *dac) {

	return SUCCESS;
}

int32_t test() {
	int32_t ret;

	pr_debug("Start\n");

	ret = init_gpios(gpios);
	void *platorm_gpio_param;
#ifdef XILINX_PLATFORM
	struct xil_gpio_init_param platorm_gpio_xil_param = {
			.device_id = GPIO_DEVICE_ID,
			.type = GPIO_PS
	};
	platorm_gpio_param = &platorm_gpio_xil_param;
#endif
#ifdef LINUX_PLATFORM
	platorm_gpio_param = NULL;
#endif
	struct gpio_init_param	ldac_param = {
			.number = GPIO_OFFSET + GPIO_LDAC_N,
			.platform_ops = gpio_ops,
			.extra = &platorm_gpio_param
	};

	// struct xil_timer_init_param xil_tmr_param = {
	// 		.type = TIMER_PS
	// };

	// struct xil_irq_init_param xil_irq_param = {
	// 		.type = IRQ_PS,
	// };
	// struct irq_init_param	irq_param = {
	// 		.irq_ctrl_id = INTC_DEVICE_ID,
	// 		.extra = &xil_irq_param
	// };
	// struct irq_ctrl_desc	*irq_crtl;

	// ret = irq_ctrl_init(&irq_crtl, &irq_param);
	// PRINT_AND_RET_ON_ERR(ret, "irq_ctrl_init failed");

#if 0
	int okk = 0;
	struct gpio_desc *ldac;
	gpio_get(&ldac, &ldac_param);
	gpio_direction_output(ldac, 1);
	while (1) {
		mdelay(100);
		gpio_set_value(ldac, okk);
		okk = !okk;
	}
#endif
	default_ad3552r_param.ldac_param = &ldac_param;
	//default_ad3552r_param.irq_crtl = irq_crtl;
	//default_ad3552r_param.tmr_extra = &xil_tmr_param;

	ret = ad3552r_init(&dac, &default_ad3552r_param);
	PRINT_AND_RET_ON_ERR(ret, "ad3552r_init failed");

	ret = gpio_set_value(gpios[GPIO_7], GPIO_LOW);
	pr_debug("Ad3552r initialized\n");

	ret = test_communication(dac);
	CHECK_TEST(ret, "test_communication");

//	test_add_sample();

//	ret = read_regs(dac);
//	PRINT_AND_RET_ON_ERR(ret, "read_regs failed");
//
//	ret = check_status(dac);
//	CHECK_TEST(ret, "check_status");
//
//	ret = test_sw_reset(dac);
//	CHECK_TEST(ret, "test_sw_reset");
//
//	ret = test_id(dac);
//	CHECK_TEST(ret, "test_id");
//
//	ret = test_regmap_multiple_transf(dac);
//	CHECK_TEST(ret, "test_regmap_multiple_transf");
//
//	ret = test_regmap_stream(dac);
//	CHECK_TEST(ret, "test_regmap_stream");
//
//	ret = test_sine_single(dac);
//	CHECK_TEST(ret, "test_sine_single");
//
//	ret = test_sine_stream(dac);
//	CHECK_TEST(ret, "test_sine_stream");
//
//	ret = test_write_dev(dac);
//	CHECK_TEST(ret, "test_write_dev");
//
//	ret = test_write_dev_precision(dac);
//	CHECK_TEST(ret, "test_write_dev_precision");
//
//	ret = test_write_dev_input(dac);
//	CHECK_TEST(ret, "test_write_dev_sync");
//
//	ret = test_write_dev_precision_input(dac);
//	CHECK_TEST(ret, "test_write_dev_precision_sync");
//
//	ret = test_crc_single(dac);
//	CHECK_TEST(ret, "test_crc_single");
//
//	ret = test_crc_stream(dac);
//	CHECK_TEST(ret, "test_crc_stream");
//
//	ret = test_crc_multiple_bytes(dac);
//	CHECK_TEST(ret, "test_crc_multiple_bytes");
//
//	ret = test_crc_stream_reg_different_values(dac);
//	CHECK_TEST(ret, "test_crc_stream_reg_different_values");

//	ret = test_timer(dac);
//	CHECK_TEST(ret, "test_timer");

	mdelay(1000);

	pr_debug("End\n");

	pr_debug("IIO started\n");

	struct iio_timer_trigger_desc *trigger;
	struct iio_timer_trigger_init_param trigger_param = {0};

	ret = iio_timer_trigger_init(&trigger, &trigger_param);
	if (IS_ERR_VALUE(ret))
		return ret;

	int ok = 0;
	while (ok)
		if (ok == -1) return -1;

	struct iio_data_buffer wr_buff = {
			.buff = (void *)DAC_DDR_BASEADDR,
			.size = 0xFFFFFFFF
	};

	struct iio_app_device devices[] = {
		IIO_APP_OUTPUT_DEVICE("ad3552r", dac, &ad3552r_iio_descriptor,
				      &wr_buff),
		IIO_APP_TRIGGER("trig", trigger,
				&iio_timer_trigger_device_desc),
	};

	ok = 1;
	while (ok) {
		ret = iio_app_run(devices, ARRAY_SIZE(devices));
		if (ret < 0)
			return ret;
	}


	return 0;
}

int main(void)
{
#ifdef LINUX_PLATFORM
	remove_gpios();
#endif
	while (1)
	{
		int ok = 1;
		uint32_t status;
		test();
		pr_debug("Test ended\n");
		ad3552r_set_dev_value(dac, AD3552R_CRC_ENABLE, 0);
		ad3552r_get_status(dac, &status, 1);
		//read_regs(dac);
		while (ok)
			;

	}

}
