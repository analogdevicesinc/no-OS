/***************************************************************************//**
 *   @file   hello_world.c
 *   @brief  Implementation of the basic example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <stdio.h>
#include "common_data.h"
#include "no_os_delay.h"
#include "ad7124.h"
#include "ad7124_regs.h"
#include "temperature_api.h"

static const struct max_spi_init_param spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

static const struct no_os_spi_init_param ad7124_spi_ip = {
	.device_id      = AD7124_SPI_DEVICE_ID,
	.max_speed_hz   = AD7124_SPI_CLK_SPEED,
	.chip_select    = AD7124_SPI_CHIP_SEL,
	.mode           = NO_OS_SPI_MODE_3,
	.platform_ops   = &max_spi_ops,
	.extra = &spi_extra,
};

static const struct ad7124_init_param ad7124_ip = {
	.spi_init = &ad7124_spi_ip,
	.regs = ad7124_regs,
	.spi_rdy_poll_cnt = AD7124_SPI_RDY_POL_CNT,
	.mode = AD7124_CONTINUOUS,
	.active_device = ID_AD7124_4,
	.ref_en = true,
	.power_mode = AD7124_HIGH_POWER,
	.setups = {
		/* Configuration for setup 0 only */
		{
			.bi_unipolar = AD7124_BIPOLAR_MODE,
			.ref_buff = AD7124_BUFF_REF,
			.ain_buff = AD7124_BUFF_AIN,
            .pga = AD7124_PGA_16,
            .ref_source = EXTERNAL_REFIN1,
		},
	},
	.chan_map = {
		/* Configuration for channel 0 only */
		{
			.channel_enable = AD7124_CH0_ENABLE,
			.setup_sel = AD7124_CH0_SETUP_SEL,
			.ain = {
                .ainp = AD7124_AIN1,
                .ainm = AD7124_AIN3,
            },
		},
	},
};

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int ad7124_app_main(void)
{
    struct no_os_uart_desc *uart_desc;
    struct ad7124_dev *dev;         /* A driver handle to pass around */
    enum ad7124_registers reg_nr;   /* Variable to iterate through registers */
    uint32_t timeout = 10000;       /* Conversion wait timeout */
    int32_t ret = 0;                /* Return value */
    int32_t sample;                 /* Stores raw value read from the ADC */
    uint32_t ch_id;                 /* Conversion channel ID */
    float temperature;

    temp_convert_t temperature_config = {
        .input = TEMP_UNIT_LSB_RAW,
        .output = TEMP_UNIT_CELSIUS,
    };

    temp_api_handle_t temp_handle = {
        .calib_value = {
            .gain = 1,
            .offset = 0
        },
    };

    ret = no_os_uart_init(&uart_desc, &uart_ip);
    if (ret)
        return ret;

    no_os_uart_stdio(uart_desc);

    printf("AD7124-4 Example Application\n\r");

    /* Initialize AD7124 device */
    ret = ad7124_setup(&dev, &ad7124_ip);
    if (ret != 0)
        return ret;

    /* Set PGA gain to 16 for setup 0 */
    ret = ad7124_reg_write_msk(dev, AD7124_CFG0_REG, AD7124_CFG_REG_PGA(4), AD7124_CFG_REG_PGA(0x7));
    if (ret != 0)
        return ret;

    /* Enable excitation current 250uA on AIN0 */
    ret = ad7124_reg_write_msk(dev, AD7124_IOCon1, AD7124_IO_CTRL1_REG_IOUT0(3), AD7124_IO_CTRL1_REG_IOUT0(0x7));
    if (ret != 0)
        return ret;

    /* Set IOUT1 on AIN4 */
    ret = ad7124_reg_write_msk(dev, AD7124_IOCon1, AD7124_IO_CTRL1_REG_IOUT_CH1(4), AD7124_IO_CTRL1_REG_IOUT_CH1(0xF));
    if (ret != 0)
        return ret;

    /* Enable excitation current 250uA on AIN4 */
    ret = ad7124_reg_write_msk(dev, AD7124_IOCon1, AD7124_IO_CTRL1_REG_IOUT1(3), AD7124_IO_CTRL1_REG_IOUT1(0x7));
    if (ret != 0)
        return ret;

    ret = ad7124_reg_write_msk(dev, AD7124_IOCon1, AD7124_IO_CTRL1_REG_PDSW, AD7124_IO_CTRL1_REG_PDSW);
    if (ret != 0)
        return ret;

    /* Enable saturation error diagnostic */
    ad7124_regs[AD7124_Error_En].value = 0x10000;

    ret = ad7124_write_register(dev, ad7124_regs[AD7124_Error_En]);
    if (ret != 0)
        return ret;

    /* Read all registers for sanity */
    for (reg_nr = AD7124_Status; (reg_nr < AD7124_REG_NO) && !(ret < 0); reg_nr++) {
        ret = ad7124_read_register(dev, &ad7124_regs[reg_nr]);
        if (ret != 0)
            return ret;
    }

    // ret = ad7124_set_odr(dev, 50, 0);
    // if (ret != 0)
    //     return ret;

    printf("    AD7124_ID: 0x%08x\n\r", ad7124_regs[AD7124_ID].value);
    printf("    AD7124_Status: 0x%08x\n\r", ad7124_regs[AD7124_Status].value);
    printf("    AD7124_ADC_Control: 0x%08x\n\r", ad7124_regs[AD7124_ADC_Control].value);
    printf("    AD7124_IOCon1: 0x%08x\n\r", ad7124_regs[AD7124_IOCon1].value);
    printf("    AD7124_IOCon2: 0x%08x\n\r", ad7124_regs[AD7124_IOCon2].value);
    printf("    AD7124_Channel_0: 0x%08x\n\r", ad7124_regs[AD7124_Channel_0].value);
    printf("    AD7124_Channel_1: 0x%08x\n\r", ad7124_regs[AD7124_Channel_1].value);
    printf("    AD7124_Config_0: 0x%08x\n\r", ad7124_regs[AD7124_Config_0].value);
    printf("    AD7124_Config_1: 0x%08x\n\r", ad7124_regs[AD7124_Config_1].value);
    printf("    AD7124_Offset_0: 0x%08x\n\r", ad7124_regs[AD7124_Offset_0].value);
    printf("    AD7124_Gain_0: 0x%08x\n\r", ad7124_regs[AD7124_Gain_0].value);
    printf("    AD7124_Error_En: 0x%08x\n\r", ad7124_regs[AD7124_Error_En].value);
    printf("    AD7124_Error: 0x%08x\n\r", ad7124_regs[AD7124_Error].value);

    printf("Conversion results:\n\r");

    while (1) {
        /* Wait for conversion */
        ret = ad7124_wait_for_conv_ready(dev, timeout);

        if (ret != 0)
            return ret;

        /* Read data from the ADC */
        sample = 0xffffffff;
        ret = ad7124_read_data(dev, &sample);

        if (ret != 0)
            return ret;

        /* Read channel ID and error from the last conversion */
		ret = ad7124_get_read_chan_id(dev, &ch_id);

        if (ret != 0)
            return ret;

        ret = ad7124_read_register(dev, &ad7124_regs[AD7124_Error]);

        if (ret != 0)
            return ret;

        ret = conversion(&temp_handle, &temperature_config, &sample, &temperature);

        printf("    Channel %d, TEMP = %.3f (C) ERROR = 0x%08x\n", ch_id, temperature, ad7124_regs[AD7124_Error].value);

        no_os_mdelay(500);
    }
}
