/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for AD74413R project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "iio_example.h"
#include "iio_ad74413r.h"
#include "iio_max14906.h"
#include "iio_adt75.h"
#include "swiot.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_gpio.h"

#include "iio.h"
#include "iio_types.h"
#include "iiod.h"

#include "no_os_timer.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"

#include "lwip/ip_addr.h"
#include "lwip_socket.h"
#include "lwip_adin1110.h"

#include "lwip/apps/lwiperf.h"

#include "hpb.h"
#include "Ext_Flash.h"
#include "spixf.h"
#include "adc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DATA_BUFFER_SIZE 1000
#define IIO_IGNORE_BUFF_OVERRUN_ERR
#define EXT_FLASH_BAUD	1000000

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(uint32_t) * 8];
uint8_t iio_data_buffer2[100 * sizeof(uint32_t) * 8];
uint8_t iio_data_buffer3[100 * sizeof(uint32_t) * 8];
uint8_t iio_data_buffer4[100 * sizeof(uint32_t) * 8];

extern int ad74413r_apply;
extern int max14906_apply;
extern int ad74413r_back;
extern int max14906_back;

extern unsigned int __HeapBase;
extern unsigned int __HeapLimit;

extern uint8_t __load_start_hpb_cs0;
extern uint8_t __load_length_hpb_cs0;
extern uint8_t __load_start_hpb_cs1;
extern uint8_t __load_length_hpb_cs1;
extern uint8_t __hpb_cs0_start;
extern uint8_t __hpb_cs1_start;
extern uint8_t __load_start_xip;
extern uint8_t __load_length_xip;

const mxc_spixf_cfg_t mx25_spixc_cfg = {
	.mode = 0,
	.ssel_pol = 0,
	.hz = EXT_FLASH_BAUD,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

struct step_param {
	struct iio_app_desc *iio_app;
	struct swiot_iio_desc *swiot;
};

int step_callback(void *arg)
{
	struct step_param *param = arg;
	struct iio_app_desc *iio_app = param->iio_app;
	struct swiot_iio_desc *swiot = param->swiot;
	int ret;

	if (swiot->mode_change) {
		swiot->mode_change = false;
		ret = iio_app_remove(iio_app);
		if (ret)
			return ret;

		return -ENOTCONN;
	}

	return 0;
}

void spixf_cfg_setup()
{
	// Disable the SPIXFC before setting the SPIXF
	MXC_SPIXF_Disable();
	MXC_SPIXF_SetSPIFrequency(EXT_FLASH_BAUD);
	MXC_SPIXF_SetMode(MXC_SPIXF_MODE_0);
	MXC_SPIXF_SetSSPolActiveLow();
	MXC_SPIXF_SetSSActiveTime(MXC_SPIXF_SYS_CLOCKS_2);
	MXC_SPIXF_SetSSActiveTime(MXC_SPIXF_SYS_CLOCKS_3);

	MXC_SPIXF_SetCmdValue(EXT_FLASH_CMD_QREAD);
	MXC_SPIXF_SetCmdWidth(MXC_SPIXF_SINGLE_SDIO);
	MXC_SPIXF_SetAddrWidth(MXC_SPIXF_QUAD_SDIO);
	MXC_SPIXF_SetDataWidth(MXC_SPIXF_WIDTH_4);
	MXC_SPIXF_SetModeClk(EXT_FLASH_QREAD_DUMMY);

	MXC_SPIXF_Set3ByteAddr();
	MXC_SPIXF_SCKFeedbackEnable();
	MXC_SPIXF_SetSCKNonInverted();
}

static int ext_flash_board_init(void)
{
    return MXC_SPIXF_Init(0, EXT_FLASH_BAUD);
}

/******************************************************************************/
static int ext_flash_board_read(uint8_t* read, unsigned len, unsigned deassert,
                                Ext_Flash_DataLine_t width)
{
    mxc_spixf_req_t req = {deassert, 0, NULL, read, (mxc_spixf_width_t)width, len, 0, 0, NULL};

    if (MXC_SPIXF_Transaction(&req) != len) {
        return E_COMM_ERR;
    }
    return E_NO_ERROR;
}

/******************************************************************************/
static int ext_flash_board_write(const uint8_t* write, unsigned len, unsigned deassert,
                                 Ext_Flash_DataLine_t width)
{
    mxc_spixf_req_t req = {deassert, 0, write, NULL, (mxc_spixf_width_t)width, len, 0, 0, NULL};

    if (MXC_SPIXF_Transaction(&req) != len) {
        return E_COMM_ERR;
    }
    return E_NO_ERROR;
}

/******************************************************************************/
static int ext_flash_clock(unsigned len, unsigned deassert)
{
    return MXC_SPIXF_Clocks(len, deassert);
}

__attribute__((section(".hpb_cs0_section"))) void test_func(void)
{
	printf("RAM");
}

__attribute__((section(".ext_flash"))) void flash_test_func(void)
{
	printf("Flash");
}

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int iio_example_main()
{
	struct tcp_pcb *pcb;
	uint32_t client_id;

	uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x60};

	mxc_hpb_mem_config_t mem;
	mxc_hpb_mem_config_t mem2;
	mxc_hpb_cfg_reg_val_t cfg_reg[1];

	int ret;
	uint32_t reg_val;
	uint32_t ndev;
	uint32_t ntrig;
	struct step_param step_p;
	struct iio_desc *iio_desc;
	struct adin1110_desc *adin1110;
	struct iio_sw_trig *sw_trig;
	struct swiot_iio_desc *swiot_iio_desc;
	struct iio_hw_trig *ad74413r_trig_desc;
	struct ad74413r_iio_desc *ad74413r_iio_desc;
	struct no_os_irq_ctrl_desc *ad74413r_irq_desc;
	struct ad74413r_iio_desc_init_param ad74413r_iio_ip;
	struct no_os_gpio_desc *tx_gpio;
	struct no_os_gpio_desc *rx_gpio;
	struct no_os_gpio_desc *ad74413r_reset_gpio;
	struct no_os_gpio_desc *ad74413r_ldac_gpio;
	struct no_os_gpio_desc *ad74413r_irq_gpio;
	struct no_os_gpio_desc *max14906_en_gpio;
	struct no_os_gpio_desc *max14906_d1_gpio;
	struct no_os_gpio_desc *max14906_d2_gpio;
	struct no_os_gpio_desc *max14906_d3_gpio;
	struct no_os_gpio_desc *max14906_d4_gpio;
	struct no_os_gpio_desc *max14906_synch_gpio;
	struct no_os_gpio_desc *adin1110_swpd_gpio;
	struct no_os_gpio_desc *adin1110_reset_gpio;
	struct no_os_gpio_desc *adin1110_tx2p4_gpio;
	struct no_os_gpio_desc *adin1110_mssel_gpio;
	struct no_os_gpio_desc *adin1110_cfg0_gpio;
	struct no_os_gpio_desc *adin1110_cfg1_gpio;
	struct no_os_gpio_desc *adin1110_int_gpio;
	struct no_os_gpio_desc *swiot_led1_gpio;
	struct no_os_gpio_desc *swiot_led2_gpio;
	struct maxq1065_desc *maxq1065;
	struct iio_app_desc *app;
	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t) * 8,
	};
	struct iio_data_buffer buff2 = {
		.buff = (void *)iio_data_buffer2,
		.size = 100 * sizeof(uint32_t) * 8,
	};
	struct iio_data_buffer buff3 = {
		.buff = (void *)iio_data_buffer3,
		.size = 100 * sizeof(uint32_t) * 8,
	};
	struct iio_data_buffer buff4 = {
		.buff = (void *)iio_data_buffer4,
		.size = 100 * sizeof(uint32_t) * 8,
	};
	struct adt75_iio_desc *adt75_iio_desc;
	struct adt75_iio_init_param adt75_iio_ip;
	struct ad74413r_init_param ad74413r_ip = {
		.chip_id = AD74413R,
		.comm_param = ad74413r_spi_ip
	};
	struct iio_app_init_param app_init_param = { 0 };

	struct max_gpio_init_param max14906_gpio_param = {
		.vssel = 1
	};
	struct no_os_gpio_init_param max14906_gpio_ip = {
		.port = 0,
		.number = 16,
		.platform_ops = &max_gpio_ops,
		.pull = NO_OS_PULL_UP,
		.extra = &max14906_gpio_param
	};

	struct no_os_gpio_init_param swiot_psu_gpio_ip = {
		.port = 3,
		.number = 9,
		.platform_ops = &max_gpio_ops,
		.pull = NO_OS_PULL_UP,
		.extra = &max14906_gpio_param
	};

	struct swiot_iio_desc_init_param swiot_ip = {
		.psu_gpio_param = swiot_psu_gpio_ip,
		.identify_gpio_param = swiot_led2_ip,
	};

	struct max14906_init_param max14906_ip = {
		.chip_address = 0,
		.comm_param = &max14906_spi_ip,
		.crc_en = true,
	};

	// struct netif *netif_desc;
	// struct max_eth_desc *eth_desc;
	// struct max_eth_param eth_param = {
	// 	.name = "e7",
	// 	.adin1110_ip = adin1110_ip,
	// };

	struct no_os_irq_ctrl_desc *ad74413r_nvic;
	struct no_os_irq_init_param ad74413r_nvic_ip = {
		.irq_ctrl_id = 0,
		.platform_ops = &max_irq_ops,
		.extra = NULL,
	};

	struct iio_sw_trig_init_param sw_trig_ip = {
		.iio_desc = iio_desc,
		.name = "sw_trig"
	};

	struct max14906_iio_desc *max14906_iio_desc;
	struct max14906_iio_desc_init_param max14906_iio_ip;

	ret = maxq1065_init(&maxq1065, &maxq1065_ip);
	if (ret)
		return ret;

	// no_os_gpio_get(&tx_gpio, &tx_perf_gpio_ip);
	// no_os_gpio_get(&rx_gpio, &rx_perf_gpio_ip);
	// no_os_gpio_direction_output(tx_gpio, 0);
	// no_os_gpio_direction_output(rx_gpio, 0);

	// cfg_reg[0].addr = 0x01000;
	// cfg_reg[0].val  = 0x801f;
        // mem.base_addr = (unsigned int)&__hpb_cs0_start;
	// mem.device_type     = MXC_HPB_DEV_HYPER_RAM;
	// mem.cfg_reg_val     = cfg_reg;
	// mem.cfg_reg_val_len = 1;
	// mem.read_cs_high    = MXC_HPB_CS_HIGH_10_5;
	// mem.write_cs_high   = MXC_HPB_CS_HIGH_10_5;
	// mem.read_cs_setup   = MXC_HPB_CS_SETUP_HOLD_16;
	// mem.write_cs_setup  = MXC_HPB_CS_SETUP_HOLD_14;
	// mem.read_cs_hold    = MXC_HPB_CS_SETUP_HOLD_5;
	// mem.write_cs_hold   = MXC_HPB_CS_SETUP_HOLD_12;
	// mem.latency_cycle   = MXC_V_HPB_MTR_LATENCY_6CLK;
	// mem.fixed_latency   = 0;

	// mem2.base_addr = (unsigned int)&__hpb_cs1_start;
	// mem2.device_type     = MXC_HPB_DEV_HYPER_RAM;
	// mem2.cfg_reg_val     = cfg_reg;
	// mem2.cfg_reg_val_len = 1;
	// mem2.read_cs_high    = MXC_HPB_CS_HIGH_10_5;
	// mem2.write_cs_high   = MXC_HPB_CS_HIGH_10_5;
	// mem2.read_cs_setup   = MXC_HPB_CS_SETUP_HOLD_16;
	// mem2.write_cs_setup  = MXC_HPB_CS_SETUP_HOLD_14;
	// mem2.read_cs_hold    = MXC_HPB_CS_SETUP_HOLD_5;
	// mem2.write_cs_hold   = MXC_HPB_CS_SETUP_HOLD_12;
	// mem2.latency_cycle   = MXC_V_HPB_MTR_LATENCY_6CLK;
	// mem2.fixed_latency   = 0;

	// memset(0x68000000, 0xA9, 10000000);
	// memset(0x78000000, 0xAF, 10000000);
        // ret = MXC_HPB_Init(&mem, &mem2);
	// memset(0x68000000, 0xA8, 10000000);
	// memset(0x78000000, 0xA0, 10000000);
	// memset(0x98000000, 0xAF, 100);

	// volatile uint8_t *ram_addr = (volatile uint8_t *)0x68000000;
	// volatile uint8_t a = *(ram_addr + 7000000);

	// memcpy(&__hpb_cs0_start, &__load_start_hpb_cs0, (uint32_t)&__load_length_hpb_cs0);
	// void (*func)(void);

	// func = (void(*)(void))((uint32_t)&__hpb_cs0_start | 1);
	// func();

	// *ram_addr = 0xA3;
	// a = *ram_addr;

	// Ext_Flash_Config_t exf_cfg = {.init  = ext_flash_board_init,
        //                           .read  = ext_flash_board_read,
        //                           .write = ext_flash_board_write,
        //                           .clock = ext_flash_clock};

	// ret = Ext_Flash_Configure(&exf_cfg);
	// if (ret)
	// 	return ret;

	// Ext_Flash_Init();
    	// Ext_Flash_Reset();
	// uint32_t flash_id = Ext_Flash_ID();
	// Ext_Flash_Erase(0x00000, Ext_Flash_Erase_64K);
	// ret = Ext_Flash_Quad(1);
	// if (ret)
	// 	return ret;


	// uint8_t flash_val[3] = {0x12, 0x34, 0xFA};
	// uint8_t flash_readback[10] = {0x0};

	// ret = Ext_Flash_Program_Page(0xF, flash_val, 3, Ext_Flash_DataLine_Quad);
	// ret = Ext_Flash_Program_Page(0xF, flash_readback, 3, Ext_Flash_DataLine_Single);

	// ret = Ext_Flash_Read(0x0, flash_readback, 10, Ext_Flash_DataLine_Single);
	// ret = Ext_Flash_Program_Page(0x0, &__load_start_xip, (uint32_t)(&__load_length_xip), Ext_Flash_DataLine_Single);
	// if (ret)
	// 	return ret;
	// ret = Ext_Flash_Read(0x0, flash_readback, 10, Ext_Flash_DataLine_Single);

	// spixf_cfg_setup();

	// *((uint8_t *)0x08000000) = 0xFE;
	// ret = Ext_Flash_Program_Page(0x0, flash_val, 3, Ext_Flash_DataLine_Quad);
	// ret = Ext_Flash_Read(0x0, flash_readback, 3, Ext_Flash_DataLine_Single);
	// flash_val[2] = 0xFE;
	// ret = Ext_Flash_Program_Page(0x0, flash_val, 3, Ext_Flash_DataLine_Quad);
	// ret = Ext_Flash_Read(0x0, flash_readback, 3, Ext_Flash_DataLine_Single);
	// ret = Ext_Flash_Read(0x0, flash_readback, 3, Ext_Flash_DataLine_Single);
	// ret = Ext_Flash_Read(0x0, flash_readback, 3, Ext_Flash_DataLine_Single);
	// ret = Ext_Flash_Read(0x0, flash_readback, 3, Ext_Flash_DataLine_Single);
	// ret = Ext_Flash_Read(0x0, flash_readback, 3, Ext_Flash_DataLine_Single);

	// func = (void(*)(void))((uint32_t)&__load_start_xip | 1);
	// func();

	// ret = Ext_Flash_Program_Page(0xF, flash_val, 3, Ext_Flash_DataLine_Quad);
	// volatile uint8_t a = *((uint8_t *)0x08000000);
	// ret = Ext_Flash_Program_Page(0x0, flash_val, 3, Ext_Flash_DataLine_Single);
	// *((uint8_t *)0x08000000) = 0xFE;
	// a = *((uint8_t *)0x08000000);

	// ret = Ext_Flash_Read(0xF, flash_readback, 10, Ext_Flash_DataLine_Single);
	// if (ret)
	// 	return ret;

	no_os_gpio_get(&max14906_d1_gpio, &max14906_d1_ip);
	no_os_gpio_get(&max14906_d2_gpio, &max14906_d2_ip);
	no_os_gpio_get(&max14906_d3_gpio, &max14906_d3_ip);
	no_os_gpio_get(&max14906_d4_gpio, &max14906_d4_ip);
	no_os_gpio_direction_output(max14906_d1_gpio, 0);
	no_os_gpio_direction_output(max14906_d2_gpio, 0);
	no_os_gpio_direction_output(max14906_d3_gpio, 0);
	no_os_gpio_direction_output(max14906_d4_gpio, 0);
	no_os_gpio_get(&max14906_en_gpio, &max14906_en_ip);
	no_os_gpio_direction_output(max14906_en_gpio, 0);

	no_os_gpio_get(&adin1110_cfg0_gpio, &adin1110_cfg0_ip);
	no_os_gpio_get(&ad74413r_reset_gpio, &ad74413r_reset_ip);
	no_os_gpio_get(&ad74413r_ldac_gpio, &ad74413r_ldac_ip);
	no_os_gpio_get(&ad74413r_irq_gpio, &ad74413r_irq_ip);
	no_os_gpio_get(&max14906_synch_gpio, &max14906_synch_ip);
	no_os_gpio_get(&adin1110_reset_gpio, &adin1110_rst_gpio_ip);
	no_os_gpio_get(&adin1110_swpd_gpio, &adin1110_swpd_ip);
	no_os_gpio_get(&adin1110_tx2p4_gpio, &adin1110_tx2p4_ip);
	no_os_gpio_get(&adin1110_mssel_gpio, &adin1110_mssel_ip);
	no_os_gpio_get(&adin1110_cfg1_gpio, &adin1110_cfg1_ip);
	no_os_gpio_get(&adin1110_int_gpio, &adin1110_int_ip);
	no_os_gpio_get(&swiot_led1_gpio, &swiot_led1_ip);
	no_os_gpio_get(&swiot_led2_gpio, &swiot_led2_ip);
	no_os_gpio_direction_output(ad74413r_reset_gpio, 1);
	no_os_gpio_direction_output(ad74413r_ldac_gpio, 1);
	no_os_gpio_direction_output(max14906_synch_gpio, 1);
	no_os_gpio_direction_output(adin1110_swpd_gpio, 1);
	no_os_gpio_direction_output(adin1110_tx2p4_gpio, 0);
	no_os_gpio_direction_output(adin1110_mssel_gpio, 1);
	no_os_gpio_direction_output(adin1110_cfg1_gpio, 1);
	no_os_gpio_direction_output(adin1110_cfg0_gpio, 1);
	no_os_gpio_direction_output(swiot_led1_gpio, 0);
	no_os_gpio_direction_output(swiot_led2_gpio, 0);
	no_os_gpio_direction_input(adin1110_int_gpio);
	no_os_gpio_direction_input(ad74413r_irq_gpio);

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	memcpy(app_init_param.lwip_param.hwaddr, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);

	if (MXC_ADC_Init())
		return -EINVAL;

	// ret = max_eth_init(&netif_desc, &eth_param);
	// if (ret)
	// 	return ret;

	// maxim_net.net = netif_desc->state;

	ret = no_os_irq_ctrl_init(&ad74413r_nvic, &ad74413r_nvic_ip);
	if (ret)
		return ret;
	
	ret = no_os_irq_enable(ad74413r_nvic, GPIO1_IRQn);
	if (ret)
		return ret;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&ad74413r_irq_desc, &ad74413r_gpio_irq_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(ad74413r_irq_desc, ad74413r_gpio_irq_ip.irq_ctrl_id, 0);
	if (ret)
		return ret;

	ad74413r_gpio_trig_ip.irq_ctrl = ad74413r_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&ad74413r_trig_desc, &ad74413r_gpio_trig_ip);
	if (ret)
		return ret;

	ret = iio_sw_trig_init(&sw_trig, &sw_trig_ip);
	if (ret)
		return ret;

	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER("sw_trig", sw_trig,
				&ad74413r_iio_trig_desc),
		IIO_APP_TRIGGER(AD74413R_GPIO_TRIG_NAME, ad74413r_trig_desc,
				&ad74413r_iio_trig_desc),
	};

	while (1) {
		adt75_iio_ip.adt75_init_param = &adt75_ip;
		max14906_iio_ip.max14906_init_param = &max14906_ip;
		ad74413r_iio_ip.ad74413r_init_param = &ad74413r_ip;

		swiot_ip.mode = 0;
		ret = swiot_iio_init(&swiot_iio_desc, &swiot_ip);
		if (ret)
			goto error;

		struct iio_app_device iio_devices[4] = {
			{
				.name = "swiot",
				.dev = swiot_iio_desc,
				.dev_descriptor = swiot_iio_desc->iio_dev,
				.read_buff = &buff2,
			},
		};

		iio_devices[0].name = "swiot";
		iio_devices[0].dev = swiot_iio_desc;
		iio_devices[0].dev_descriptor = swiot_iio_desc->iio_dev;
		iio_devices[0].read_buff = &buff2;

		app_init_param.devices = iio_devices;
		app_init_param.nb_devices = 1;
		app_init_param.trigs = trigs;
		app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
		app_init_param.uart_init_params = adin1110_uart_ip;
		app_init_param.post_step_callback = step_callback;
		app_init_param.lwip_param.platform_ops = &adin1110_lwip_ops;
		app_init_param.lwip_param.mac_param = &adin1110_ip;
		app_init_param.lwip_param.extra = NULL;

		ret = iio_app_init(&app, app_init_param);
		if (ret) {
			goto error;
		}

		swiot_iio_desc->adin1110 = app->lwip_desc->mac_desc;
		step_p.swiot = swiot_iio_desc;
		step_p.iio_app = app;
		app->arg = &step_p;	

		ret = iio_app_run(app);
		if (ret != -ENOTCONN) {
			goto error;
		}

		ndev = 1;
		ntrig = 0;
		max14906_iio_ip.channel_configs = &swiot_iio_desc->max14906_configs;
		ad74413r_iio_ip.channel_configs = &swiot_iio_desc->ad74413r_configs;
		/* Probe the drivers in the run mode */
		ret = max14906_iio_init(&max14906_iio_desc, &max14906_iio_ip, false);
		if (!ret) {
			iio_devices[1].name = "max14906";
			iio_devices[1].dev = max14906_iio_desc;
			iio_devices[1].dev_descriptor = max14906_iio_desc->iio_dev;
			iio_devices[1].read_buff = &buff3;
			ndev++;
		} else {
			goto error;
		}

		ret = adt75_iio_init(&adt75_iio_desc, &adt75_iio_ip);
		if (!ret) {
			iio_devices[2].name = "adt75";
			iio_devices[2].dev = adt75_iio_desc;
			iio_devices[2].dev_descriptor = adt75_iio_desc->iio_dev;
			iio_devices[2].read_buff = &buff4;
			ndev++;
		} else {
			goto error;
		}

		ret = ad74413r_iio_init(&ad74413r_iio_desc, &ad74413r_iio_ip, false);
		if (!ret) {
			iio_devices[3].name = "ad74413r";
			iio_devices[3].dev = ad74413r_iio_desc;
			iio_devices[3].dev_descriptor = ad74413r_iio_desc->iio_dev;
			iio_devices[3].read_buff = &buff;
			ntrig++;
			ndev++;
		} else {
			goto error;
		}

		swiot_iio_desc->ad74413r = ad74413r_iio_desc;
		swiot_iio_desc->max14906 = max14906_iio_desc;
		swiot_iio_desc->mode = 1;

		app_init_param.devices = iio_devices;
		app_init_param.nb_devices = ndev;
		app_init_param.trigs = trigs;
		app_init_param.nb_trigs = 2;
		app_init_param.uart_init_params = adin1110_uart_ip;
		app_init_param.post_step_callback = step_callback;

		no_os_gpio_set_value(swiot_led2_gpio, 1);
		ret = iio_app_init(&app, app_init_param);
		if (ret) {
			goto error;
		}
		no_os_gpio_set_value(swiot_led2_gpio, 0);

		ad74413r_trig_desc->iio_desc = app->iio_desc;
		swiot_iio_desc->adin1110 = app->lwip_desc->mac_desc;
		step_p.swiot = swiot_iio_desc;
		step_p.iio_app = app;
		app->arg = &step_p;

		no_os_gpio_set_value(max14906_en_gpio, 1);
		ret = iio_app_run(app);
		if (ret != -ENOTCONN) {
			no_os_gpio_set_value(max14906_en_gpio, 0);
			goto error;
		}

		no_os_gpio_set_value(max14906_en_gpio, 0);
		ret = ad74413r_iio_remove(ad74413r_iio_desc);
		if (ret) {
			goto error;
		}

		ret = max14906_iio_remove(max14906_iio_desc);
		if (ret) {
			goto error;
		}

		ret = adt75_iio_remove(adt75_iio_desc);
		if (ret) {
			goto error;
		}

		ret = swiot_iio_remove(swiot_iio_desc);
		if (ret) {
			goto error;
		}
	}

error:
	printf("Out err: %d\n", ret);
	return ret;
}
