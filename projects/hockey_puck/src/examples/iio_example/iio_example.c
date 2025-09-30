/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Hockey Puck IIO server application.
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "common_data.h"
#include "common.h"
#include "iio_app.h"
#include "iio_ad7124.h"
#include "ad7124.h"
#include "ad7124_regs.h"
#include "max42500.h"
#include "max42500_regs.h"
#include "adin1110.h"
#include "lwip_adin1110.h"

const struct max_i2c_init_param max42500_i2c_ip = {
    .vssel = MXC_GPIO_VSSEL_VDDIOH,
};

const struct max_gpio_init_param max42500_gpio_extra = {
    .vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max42500_init_param max42500_ip = {
    .regs       = max42500_regs,
	.comm_param = {
        .device_id      = MAX42500_I2C_DEVICE_ID,
        .max_speed_hz   = MAX42500_I2C_CLK_SPEED,
        .slave_address  = MAX42500_I2C_ADDR,
        .platform_ops   = &max_i2c_ops,
        .extra          = &max42500_i2c_ip,
    },
    .timer_param = {
        .id             = MAX42500_TIMER_ID,
        .freq_hz        = MAX42500_TIMER_ID,
        .ticks_count    = MAX42500_TIMER_COUNT,
        .platform_ops   = &max_timer_ops,
        .extra          = NULL,
    },
    .irq_param = {
        .irq_ctrl_id    = MAX42500_IRQ_ID,
        .platform_ops   = &max_irq_ops,
        .extra          = NULL,
    },
    .en0_param = {
        .port           = MAX42500_EN0_PORT,
        .number         = MAX42500_EN0_PIN,
        .pull           = NO_OS_PULL_UP,
        .platform_ops   = &max_gpio_ops,
        .extra          = &max42500_gpio_extra,
    },
    .en1_param = {
        .port           = MAX42500_EN1_PORT,
        .number         = MAX42500_EN1_PIN,
        .pull           = NO_OS_PULL_UP,
        .platform_ops   = &max_gpio_ops,
        .extra          = &max42500_gpio_extra,
    },
    .addr_param = {
        .port           = MAX42500_ADDR_PORT,
        .number         = MAX42500_ADDR_PIN,
        .pull           = NO_OS_PULL_DOWN,
        .platform_ops   = &max_gpio_ops,
        .extra          = &max42500_gpio_extra,
    },
    .addr_sel   = MAX42500_ADDR_SEL,
    .pece       = MAX42500_PECE,
    .vmon_en    = MAX42500_VMON_EN,
    .vmon_vmpd  = MAX42500_VMON_VMPD,
    .reset_map  = MAX42500_RESET_MAP,
    .wd_mode    = MAX42500_WD_MODE,
    .wd_en      = MAX42500_WD_EN,
};

const struct no_os_gpio_init_param adin1110_rst_gpio_ip = {
	.port = ADIN1110_RESET_PORT,
	.number = ADIN1110_RESET_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param){
		.vssel =0
	},
};

const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = ADIN1110_SPI_DEVICE_ID,
	.max_speed_hz = ADIN1110_SPI_CLK_SPEED,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = ADIN1110_SPI_CHIP_SEL,
	.extra = &(struct max_spi_init_param){
        .num_slaves = 1,
        .polarity = SPI_SS_POL_LOW,
        .vssel = MXC_GPIO_VSSEL_VDDIO,
    },
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_rst_gpio_ip,
	.mac_address = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80},
	.append_crc = false,
	.oa_tc6_spi = true,
	.oa_tc6_prote = true,
};

struct lwip_network_param lwip_ip = {
	.platform_ops = &adin1110_lwip_ops,
	.mac_param = &adin1110_ip,
};

static const struct no_os_spi_init_param ad7124_spi_ip = {
	.device_id      = AD7124_SPI_DEVICE_ID,
	.max_speed_hz   = AD7124_SPI_CLK_SPEED,
	.chip_select    = AD7124_SPI_CHIP_SEL,
	.mode           = NO_OS_SPI_MODE_3,
	.platform_ops   = &max_spi_ops,
	.extra = &(struct max_spi_init_param){
        .num_slaves = 1,
        .polarity = SPI_SS_POL_LOW,
        .vssel = MXC_GPIO_VSSEL_VDDIOH,
    },
};

static const struct ad7124_init_param ad7124_ip = {
	.spi_init = &ad7124_spi_ip,
	.regs = ad7124_regs,
	.spi_rdy_poll_cnt = AD7124_SPI_RDY_POL_CNT,
	.mode = AD7124_CONTINUOUS,
	.active_device = ID_AD7124_4,
	.ref_en = false,
	.power_mode = AD7124_HIGH_POWER,
	.setups = {
		/* Configuration for setup 0 only */
		{
			AD7124_BIPOLAR_MODE,
			AD7124_BUFF_REF,
			AD7124_BUFF_AIN,
			EXTERNAL_REFIN1
		},
	},
	.chan_map = {
		/* Configuration for channel 0 only */
		{
			AD7124_CH0_ENABLE,
			AD7124_CH0_SETUP_SEL,
			AD7124_CH0_AIN_PINS,
		},
	},
};

struct iio_server_t {
    struct iio_app_desc *app;                   /* IIO app descriptor */
    struct iio_app_init_param app_init_param;   /* IIO app init parameters */
    struct ad7124_dev *ad7124_desc;             /* AD7124-4 descriptor */
    struct iio_data_buffer ad7124_buff;         /* AD7124-4 IIO buffer */
    struct max42500_desc *max42500_desc;        /* MAX42500 descriptor */
};

static uint8_t ad7124_iio_buff[AD7124_IIO_BUFF_SIZE];

static struct iio_server_t iio_server = {
    .app                = NULL,
    .app_init_param     = { 0 },
    .ad7124_desc        = NULL,
    .ad7124_buff        = {
        .buff = (void *)ad7124_iio_buff,
        .size = AD7124_IIO_BUFF_SIZE
    },
    .max42500_desc      = NULL,
};

/* ADIN1110 ethernet MAC address */
static const uint8_t adin1110_mac_address[NETIF_MAX_HWADDR_LEN] = {
    0x00, 0x18, 0x80, 0x03, 0x25, 0x60
};

/*!
 * @brief   Setup MAX42500.
 */
static int32_t setup_max42500(void)
{
    int32_t err_code;

    err_code = max42500_init(&iio_server.max42500_desc, &max42500_ip);

    DEBUG_RESULT("Initializing MX42500 device...", err_code, ERR_CODE_NO_ERR);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_CONFIG1]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_CONFIG2]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_VMON]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_RSTMAP]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_STATOV]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_STATUV]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_STATOFF]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_VIN2]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_VIN3]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_VIN4]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_OVUV2]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_OVUV3]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_OVUV4]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_WDCDIV]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_WDCFG1]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_WDCFG2]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_WDKEY]);

    err_code = max42500_reg_read(iio_server.max42500_desc,
                                 &iio_server.max42500_desc->regs[MAX42500_RSTCTRL]);

    DEBUG_MESSAGE("\tMAX42500_CONFIG1: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_CONFIG1].value);

    DEBUG_MESSAGE("\tMAX42500_CONFIG2: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_CONFIG2].value);

    DEBUG_MESSAGE("\tMAX42500_VMON: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_VMON].value);

    DEBUG_MESSAGE("\tMAX42500_RSTMAP: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_RSTMAP].value);

    DEBUG_MESSAGE("\tMAX42500_STATOV: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_STATOV].value);

    DEBUG_MESSAGE("\tMAX42500_STATUV: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_STATUV].value);

    DEBUG_MESSAGE("\tMAX42500_STATOFF: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_STATOFF].value);

    DEBUG_MESSAGE("\tMAX42500_VIN2: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_VIN2].value);

    DEBUG_MESSAGE("\tMAX42500_VIN3: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_VIN3].value);

    DEBUG_MESSAGE("\tMAX42500_VIN4: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_VIN4].value);

    DEBUG_MESSAGE("\tMAX42500_OVUV2: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_OVUV2].value);

    DEBUG_MESSAGE("\tMAX42500_OVUV3: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_OVUV3].value);

    DEBUG_MESSAGE("\tMAX42500_OVUV4: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_OVUV4].value);

    DEBUG_MESSAGE("\tMAX42500_WDCDIV: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_WDCDIV].value);

    DEBUG_MESSAGE("\tMAX42500_WDCFG1: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_WDCFG1].value);

    DEBUG_MESSAGE("\tMAX42500_WDCFG2: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_WDCFG2].value);

    DEBUG_MESSAGE("\tMAX42500_WDKEY: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_WDKEY].value);

    DEBUG_MESSAGE("\tMAX42500_RSTCTRL: 0x%02X",
                  iio_server.max42500_desc->regs[MAX42500_RSTCTRL].value);

    return err_code;
}

/*!
 * @brief   Setup AD7124.
 */
static int32_t setup_ad7124(void)
{
    int32_t err_code;
    uint32_t reg_value;

    err_code = ad7124_setup(&iio_server.ad7124_desc, &ad7124_ip);

    DEBUG_RESULT("Initializing AD7124 device...", err_code, ERR_CODE_NO_ERR);

    err_code = ad7124_reg_write_msk(iio_server.ad7124_desc,
                                    AD7124_CFG0_REG,
                                    AD7124_CFG_REG_PGA(AD7124_PGA_GAIN),
                                    AD7124_CFG_REG_PGA(0x7));

    DEBUG_RESULT("Setting AD7124 PGA gain...", err_code, ERR_CODE_NO_ERR);

    err_code = ad7124_reg_write_msk(iio_server.ad7124_desc,
                                    AD7124_IOCon1,
                                    AD7124_IO_CTRL1_REG_IOUT0(AD7124_IEXC_IOUT0),
                                    AD7124_IO_CTRL1_REG_IOUT0(0x7));

    DEBUG_RESULT("Enabling AD7124 IEXC on AIN0...", err_code, ERR_CODE_NO_ERR);

    err_code = ad7124_reg_write_msk(iio_server.ad7124_desc,
                                    AD7124_IOCon1,
                                    AD7124_IO_CTRL1_REG_PDSW,
                                    AD7124_IO_CTRL1_REG_PDSW);

    DEBUG_RESULT("Setting AD7124 PDSW on PSW...", err_code, ERR_CODE_NO_ERR);

	err_code = ad7124_read_register2(iio_server.ad7124_desc,
                                    AD7124_IOCon1,
                                    &reg_value);

    DEBUG_RESULT("Reading AD7124 IO_CONTROL_1...", err_code, ERR_CODE_NO_ERR);
    DEBUG_MESSAGE("\tAD7124_IO_CONTROL_1: 0x%X", reg_value);

    return err_code;
}

/*!
 * @brief   IIO server application.
 * @return  ret - Result of the execution. If working correctly, it will
 *          continuously execute function iio_app_run and will not return.
 */
int iio_server_app(void)
{
    int32_t err_code;

    DEBUG_MESSAGE("Running Hockey Puck IIO server application...");

    err_code = setup_ad7124();

    /* Populate devices supported by the IIO server */
    struct iio_app_device iio_devices[] = {
        IIO_APP_DEVICE("ad7124-4",
                       iio_server.ad7124_desc,
                       &iio_ad7124_device,
                       &iio_server.ad7124_buff,
                       NULL,
                       NULL),
    };

    /* Populate IIO initialization parameters */
    memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
    memcpy(iio_server.app_init_param.lwip_param.hwaddr, adin1110_mac_address,
           NETIF_MAX_HWADDR_LEN);

    iio_server.app_init_param.devices = iio_devices;
    iio_server.app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
    iio_server.app_init_param.uart_init_params = uart_ip;
    iio_server.app_init_param.lwip_param.platform_ops = &adin1110_lwip_ops;
    iio_server.app_init_param.lwip_param.mac_param = &adin1110_ip;
    iio_server.app_init_param.lwip_param.extra = NULL;

    /* Initialize IIO app */
    err_code = iio_app_init(&iio_server.app, iio_server.app_init_param);
    DEBUG_RESULT("Initializing IIO application...", err_code, ERR_CODE_NO_ERR);

    if (err_code == ERR_CODE_NO_ERR) {
        DEBUG_MESSAGE("Running IIO application...");

        /* Run IIO app. This call should not return */
        err_code = iio_app_run(iio_server.app);
    }

    DEBUG_RESULT("IIO application runtime return...", err_code, ERR_CODE_NO_ERR);

    /* Post-runtime failure cleanup */
    iio_app_remove(iio_server.app);
    ad7124_remove(iio_server.ad7124_desc);
    max42500_remove(iio_server.max42500_desc);

    return err_code;
}

/*!
 * @brief Main function execution for Hockey Puck IIO server application.
 */
int iio_example_main(void)
{
    /* Debug UART descriptor */
    struct no_os_uart_desc *debug_uart_desc;

    /* Initialize UART for printf debugging */
    no_os_uart_init(&debug_uart_desc, &debug_uart_ip);
    no_os_uart_stdio(debug_uart_desc);

    return iio_server_app();
}
