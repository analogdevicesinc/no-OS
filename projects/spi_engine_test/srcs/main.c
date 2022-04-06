/***************************************************************************//**
 *   @file   ad3552r_fmcz/srcs/main.c
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

#include <inttypes.h>
#include "app_config.h"
#include "parameters.h"
#include "no-os/error.h"
#include "no-os/print_log.h"
#include "no-os/spi.h"
#include "no-os/gpio.h"
#include "no-os/util.h"
#include "no-os/delay.h"
#include "spi_engine.h"
#include "spi_extra.h"
#include "clk_axi_clkgen.h"
#include "gpio_extra.h"
#include "ad3552r.h"

#include <xparameters.h>
#include <xil_cache.h>

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_ad3552r.h"

static uint8_t data_buffer[MAX_BUFF_SAMPLES];

#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD3552R_SPI_ENGINE_BASEADDR		XPAR_SPI_AD3552R_DAC_AXI_REGMAP_BASEADDR
#define AD3552R_SPI_CS					0
#define AD3552R_SPI_ENG_REF_CLK_FREQ_HZ	120000000
#define AD3552R_SPI_MAX_FREQ_HZ			1200000
#define AD3552R_CLKGEN_BASEADDR			XPAR_SPI_CLKGEN_BASEADDR
#define AD3552R_CLKGEN_CLK_FREQ_HZ		100000000

#define AD3552R_REG_READ(x)				((1 << 7) | (x & 0x7F))

struct test_init_param {
	/** SPI */
	struct spi_init_param *spi_init;
	/** GPIO */
	struct gpio_init_param *gpio_resetn;
	/** Clock gen for hdl design init structure */
	struct axi_clkgen_init *clkgen_init;
	/** Clock generator rate */
	uint32_t spi_clkgen_rate;
	/** Register access speed */
	uint32_t reg_access_speed;
	/** Device id */
	enum ad3552r_id device_id;
};


struct test_desc {
	struct spi_desc *spi;
	struct gpio_desc *reset;
	struct axi_clkgen *test_clkgen;
	uint8_t chip_id;
};

static struct xil_gpio_init_param xil_gpio_param = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PS
};

static struct gpio_init_param default_gpio_param = {
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

int32_t init_gpios_to_defaults()
{
	const uint8_t gpios_initial_value[][2] = {
		[GPIO_RESET_N] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_LDAC_N] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_SPI_QPI] = {GPIO_OUT, GPIO_LOW},
		[GPIO_ALERT_N] = {GPIO_IN, 0},
		[GPIO_SYNC_EVENTS] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_RED] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_GREEN] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_BLUE] = {GPIO_OUT, GPIO_HIGH},
	};
	struct gpio_desc *gpio;
	struct gpio_init_param param = default_gpio_param;
	uint32_t i;
	int32_t	 err;

	for (i = 0; i < TOTAL_GPIOS; i++) {
		param.number = GPIO_OFFSET + i;
		err = gpio_get(&gpio, &param);
		if (IS_ERR_VALUE(err))
			return err;
		if (gpios_initial_value[i][0] == GPIO_IN)
			err = gpio_direction_input(gpio);
		else
			err = gpio_direction_output(gpio,
						    gpios_initial_value[i][1]);

		if (IS_ERR_VALUE(err))
			return err;

		gpio_remove(gpio);
	}

	return SUCCESS;
}

int32_t test_spi_reg_write(struct test_desc *dev, uint8_t addr, uint8_t *val)
{
	int32_t ret;
	uint8_t buf[3];

	buf[0] = addr;
	buf[1] = *val;
	buf[2] = 0x00;

	ret = spi_write_and_read(dev->spi, buf, 3);

	return ret;
}

int32_t test_spi_reg_read(struct test_desc *dev, uint8_t addr, uint8_t *val)
{
	int32_t ret;
	uint8_t buf[3];

	buf[0] = AD3552R_REG_READ(addr);
	buf[1] = 0xFF;
	buf[2] = 0xFF;

	ret = spi_write_and_read(dev->spi, buf, 2);
	*val = buf[1];

	return ret;
}

int32_t test_spi_set_dual(struct test_desc *dev)
{
	int32_t ret;
	uint8_t buf[3];

	buf[0] = 0x0f;
	buf[1] = 0x10;
	buf[2] = 0x00;

	ret = spi_write_and_read(dev->spi, buf, 4);

	return ret;
}

int32_t test_spi_set_single_inst(struct test_desc *dev)
{
	int32_t ret;
	uint8_t buf[3];

	buf[0] = 0x01;
	buf[1] = 0x40;
	buf[2] = 0x40;

	ret = spi_write_and_read(dev->spi, buf, 4);

	return ret;
}

int32_t test_init(struct test_desc **desc,
				struct test_init_param *init_param)
{
	struct test_desc *dev;
	uint8_t val;
	int32_t ret;

	if (!desc || !init_param)
		return -EINVAL;

	dev = (struct test_desc *)malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = axi_clkgen_init(&dev->test_clkgen, init_param->clkgen_init);
	if (ret != SUCCESS) {
		pr_err("error: %s: axi_clkgen_init() failed\n",
		       init_param->clkgen_init->name);
		goto error_dev;
	}

	ret = axi_clkgen_set_rate(dev->test_clkgen, init_param->spi_clkgen_rate);
	if (ret != SUCCESS) {
		pr_err("error: %s: axi_clkgen_set_rate() failed\n",
		       init_param->clkgen_init->name);
		goto error_dev;
	}

	ret = spi_init(&dev->spi, init_param->spi_init);
	if (ret != SUCCESS)
		goto err_spi;

	ret = gpio_get_optional(&dev->reset, init_param->gpio_resetn);
	if (ret != SUCCESS)
		goto err_reset;

	ret = gpio_direction_output(dev->reset, GPIO_HIGH);
	if (ret != SUCCESS)
		goto err_reset;

	gpio_set_value(dev->reset, GPIO_LOW);
	mdelay(1);
	gpio_set_value(dev->reset, GPIO_HIGH);

	spi_engine_set_transfer_width(dev->spi, 8);

//	ret = test_spi_set_single_inst(dev);
//	if (ret != SUCCESS) {
//		pr_err("Fail to set to single instruction: %"PRIi32"\n", ret);
//		goto err_reset;
//	};

	ret = test_spi_set_dual(dev);
	if (ret != SUCCESS) {
		pr_err("Fail to set IC to Dual: %"PRIi32"\n", ret);
		goto err_reset;
	};

	val = 0xa5;
	ret = test_spi_reg_write(dev, 0x29, &val);
	if (ret != SUCCESS) {
		pr_err("Fail read PRODUCT_ID_L: %"PRIi32"\n", ret);
		goto err_reset;
	};

	ret = test_spi_reg_read(dev, 0x29, &val);
	if (ret != SUCCESS) {
		pr_err("Fail read PRODUCT_ID_L: %"PRIi32"\n", ret);
		goto err_reset;
	};

	return SUCCESS;
err_reset:
	gpio_remove(dev->reset);
err_spi:
	spi_remove(dev->spi);
error_dev:
	free(dev);

	return FAILURE;
}

int main()
{
	struct test_desc *dev;
	int32_t ret;

	pr_debug("Hey, welcome to spi_engine test\n");

	struct spi_engine_init_param spi_engine_init  = {
		.ref_clk_hz = AD3552R_SPI_ENG_REF_CLK_FREQ_HZ,
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD3552R_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 32,
		.number_of_lines = SPI_DUAL,
	};

	struct spi_init_param spi_init = {
		.chip_select = AD3552R_SPI_CS,
		.max_speed_hz = AD3552R_SPI_MAX_FREQ_HZ,
		.mode = SPI_MODE_0,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_engine_init,
	};

	struct axi_clkgen_init clkgen_test_init = {
		.base = AD3552R_CLKGEN_BASEADDR,
		.name = "test_clkgen",
		.parent_rate = AD3552R_CLKGEN_CLK_FREQ_HZ
	};

	struct gpio_init_param reset_param = {
		.number = GPIO_OFFSET + GPIO_RESET_N,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param
	};

	struct test_init_param test_init_param = {
		.device_id = 1,
		.spi_init = &spi_init,
		.gpio_resetn = &reset_param,
		.clkgen_init = &clkgen_test_init,
		.spi_clkgen_rate = AD3552R_SPI_ENG_REF_CLK_FREQ_HZ
	};

	ret = test_init(&dev, &test_init_param);
	if (ret != SUCCESS) {
		pr_err("ad3552r_init failed with code: %"PRIi32"\n", ret);
		return ret;
	}

	pr_debug("Bye\n");

	return 0;
}
