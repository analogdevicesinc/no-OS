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

#include "iio_example.h"
#include "iio_ad74413r.h"
#include "iio_max14906.h"
#include "common_data.h"
#include "no_os_util.h"

#include "iio.h"
#include "iio_types.h"
#include "iiod.h"

#include "no_os_timer.h"
#include "max_eth.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DATA_BUFFER_SIZE 400

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(uint32_t) * 8];
uint8_t iio_data_buffer2[DATA_BUFFER_SIZE * sizeof(uint32_t) * 8];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
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

	int ret;
	uint32_t reg_val;
	struct iio_desc *iio_desc;
	struct adin1110_desc *adin1110;
	struct iio_hw_trig *ad74413r_trig_desc;
	struct ad74413r_iio_desc *ad74413r_iio_desc;
	struct no_os_irq_ctrl_desc *ad74413r_irq_desc;
	struct ad74413r_iio_desc_init_param ad74413r_iio_ip;
	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t) * 8
	};
	struct iio_data_buffer buff2 = {
		.buff = (void *)iio_data_buffer2,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t) * 8
	};
	struct ad74413r_init_param ad74413r_ip = {
		.chip_id = AD74412R,
		.comm_param = ad74413r_spi_ip
	};
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
	struct max14906_init_param max14906_ip = {
		.chip_address = 0,
		.comm_param = &max14906_spi_ip,
	};

	struct no_os_timer_init_param eth_tick_param = {
		.id = 0,
		.freq_hz = 64000,
		.ticks_count = 600,
		.platform_ops = &max_timer_ops,
		.extra = NULL,
	};

	struct netif *netif_desc;
	struct max_eth_desc *eth_desc;
	struct max_eth_param eth_param = {
		.name = "e7",
		.link_callback = NULL,
		.adin1110_ip = adin1110_ip,
		.tick_param = eth_tick_param,
	};

	struct no_os_irq_ctrl_desc *ad74413r_nvic;
	struct no_os_irq_init_param ad74413r_nvic_ip = {
		.irq_ctrl_id = 0,
		.platform_ops = &max_irq_ops,
		.extra = NULL,
	};

	struct max14906_iio_desc *max14906_iio_desc;
	struct max14906_iio_desc_init_param max14906_iio_ip;

	ret = max_eth_init(&netif_desc, &eth_param);
	if (ret)
		return ret;

	eth_desc = netif_desc->state;
	ret = eth_desc->noos_net.socket_open(eth_desc, 0, 0, 100);
	if (ret)
		return ret;

	ret = eth_desc->noos_net.socket_bind(eth_desc, 0, 30431);
	if (ret)
		return ret;

	uint8_t *demo_str = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE context [<!ELEMENT context (device | context-attribute)*><!ELEMENT context-attribute EMPTY><!ELEMENT device (channel | attribute | debug-attribute | buffer-attribu \
				te)*><!ELEMENT channel (scan-element?, attribute*)><!ELEMENT attribute EMPTY><!ELEMENT scan-element EMPTY><!ELEMENT debug-attribute EMPTY><!ELEMENT buffer-attribute EMPTY><!ATTLIST context name CDATA #REQUIRED d \
				escription CDATA #IMPLIED><!ATTLIST context-attribute name CDATA #REQUIRED value CDATA #REQUIRED><!ATTLIST device id CDATA #REQUIRED name CDATA #IMPLIED><!ATTLIST channel id CDATA #REQUIRED type (input|output) # \
				REQUIRED name CDATA #IMPLIED><!ATTLIST scan-element index CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED><!ATTLIST attribute name CDATA #REQUIRED filename CDATA #IMPLIED><!ATTLIST debug-attribute na \
				me CDATA #REQUIRED><!ATTLIST buffer-attribute name CDATA #REQUIRED>]><context name=\"xml\" description=\"no-OS master-088abd736\" ><device id=\"iio:device0\" name=\"adc_demo\"><channel id=\"voltage0\" name=\"adc_in_ch0\" ty \
				pe=\"input\" ><scan-element index=\"0\" format=\"le:s16/16>>0\" /><attribute name=\"adc_channel_attr\" filename=\"in_voltage0_adc_channel_attr\" /></channel><channel id=\"voltage1\" name=\"adc_in_ch1\" type=\"input\" ><scan-ele \
				ment index=\"1\" format=\"le:s16/16>>0\" /><attribute name=\"adc_channel_attr\" filename=\"in_voltage1_adc_channel_attr\" /></channel><attribute name=\"adc_global_attr\" /><debug-attribute name=\"direct_reg_access\" /></dev \
				ice><device id=\"iio:device1\" name=\"dac_demo\"><channel id=\"voltage0\" name=\"dac_out_ch0\" type=\"output\" ><scan-element index=\"0\" format=\"le:s16/16>>0\" /><attribute name=\"dac_channel_attr\" filename=\"out_voltage0_dac \
				_channel_attr\" /></channel><channel id=\"voltage1\" name=\"dac_out_ch1\" type=\"output\" ><scan-element index=\"1\" format=\"le:s16/16>>0\" /><attribute name=\"dac_channel_attr\" filename=\"out_voltage1_dac_channel_attr\" />< \
				/channel><attribute name=\"dac_global_attr\" /><debug-attribute name=\"direct_reg_access\" /></device></context>";

	uint8_t *demo_ctx = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE context [<!ELEMENT context (device | context-attribute)*><!ELEMENT context-attribute EMPTY><!ELEMENT device (channel | attribute | debug-attribute | buffer-attribute)*><!ELEMENT channel (scan-element?, attribute*)><!ELEMENT attribute EMPTY><!ELEMENT scan-element EMPTY><!ELEMENT debug-attribute EMPTY><!ELEMENT buffer-attribute EMPTY><!ATTLIST context name CDATA #REQUIRED description CDATA #IMPLIED><!ATTLIST context-attribute name CDATA #REQUIRED value CDATA #REQUIRED><!ATTLIST device id CDATA #REQUIRED name CDATA #IMPLIED><!ATTLIST channel id CDATA #REQUIRED type (input|output) #REQUIRED name CDATA #IMPLIED><!ATTLIST scan-element index CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED><!ATTLIST attribute name CDATA #REQUIRED filename CDATA #IMPLIED value CDATA #IMPLIED><!ATTLIST debug-attribute name CDATA #REQUIRED value CDATA #IMPLIED><!ATTLIST buffer-attribute name CDATA #REQUIRED value CDATA #IMPLIED>]><context name=\"serial\" description=\"/dev/ttyUSB1: FT230X Basic UART - D3091LE6\" ><context-attribute name=\"uri\" value=\"serial:/dev/ttyUSB1,115200,8n2n\" /><device id=\"iio:device0\" name=\"ad74413r\" ><channel id=\"voltage0\" name=\"config_ch0\" type=\"input\" ><attribute name=\"enabled\" filename=\"in_voltage0_enabled\" value=\"0\" /><attribute name=\"function_cfg\" filename=\"in_voltage0_function_cfg\" value=\"high_z\" /><attribute name=\"function_cfg_available\" filename=\"function_cfg_available\" value=\"ERROR\" /></channel><channel id=\"voltage1\" name=\"config_ch1\" type=\"input\" ><attribute name=\"enabled\" filename=\"in_voltage1_enabled\" value=\"0\" /><attribute name=\"function_cfg\" filename=\"in_voltage1_function_cfg\" value=\"high_z\" /><attribute name=\"function_cfg_available\" filename=\"function_cfg_available\" value=\"ERROR\" /></channel><channel id=\"voltage2\" name=\"config_ch2\" type=\"input\" ><attribute name=\"enabled\" filename=\"in_voltage2_enabled\" value=\"0\" /><attribute name=\"function_cfg\" filename=\"in_voltage2_function_cfg\" value=\"high_z\" /><attribute name=\"function_cfg_available\" filename=\"function_cfg_available\" value=\"ERROR\" /></channel><channel id=\"voltage3\" name=\"config_ch3\" type=\"input\" ><attribute name=\"enabled\" filename=\"in_voltage3_enabled\" value=\"0\" /><attribute name=\"function_cfg\" filename=\"in_voltage3_function_cfg\" value=\"high_z\" /><attribute name=\"function_cfg_available\" filename=\"function_cfg_available\" value=\"ERROR\" /></channel><attribute name=\"apply\" value=\"0\" /></device><device id=\"iio:device1\" name=\"max14906\" ><channel id=\"voltage0\" type=\"input\" ><attribute name=\"function\" filename=\"in_voltage0_function\" value=\"direction_out\" /><attribute name=\"function_available\" filename=\"function_available\" value=\"direction_out direction_in direction_high_z\" /><attribute name=\"IEC_type\" filename=\"in_IEC_type\" value=\"Type_1_3\" /><attribute name=\"IEC_type_available\" filename=\"in_IEC_type_available\" value=\"Type_1_3 Type_2\" /><attribute name=\"enabled\" filename=\"in_voltage0_enabled\" value=\"0\" /></channel><channel id=\"voltage1\" type=\"input\" ><attribute name=\"function\" filename=\"in_voltage1_function\" value=\"direction_out\" /><attribute name=\"function_available\" filename=\"function_available\" value=\"direction_out direction_in direction_high_z\" /><attribute name=\"IEC_type\" filename=\"in_IEC_type\" value=\"Type_1_3\" /><attribute name=\"IEC_type_available\" filename=\"in_IEC_type_available\" value=\"Type_1_3 Type_2\" /><attribute name=\"enabled\" filename=\"in_voltage1_enabled\" value=\"0\" /></channel><channel id=\"voltage2\" type=\"input\" ><attribute name=\"function\" filename=\"in_voltage2_function\" value=\"direction_out\" /><attribute name=\"function_available\" filename=\"function_available\" value=\"direction_out direction_in direction_high_z\" /><attribute name=\"IEC_type\" filename=\"in_IEC_type\" value=\"Type_1_3\" /><attribute name=\"IEC_type_available\" filename=\"in_IEC_type_available\" value=\"Type_1_3 Type_2\" /><attribute name=\"enabled\" filename=\"in_voltage2_enabled\" value=\"0\" /></channel><channel id=\"voltage3\" type=\"input\" ><attribute name=\"function\" filename=\"in_voltage3_function\" value=\"direction_out\" /><attribute name=\"function_available\" filename=\"function_available\" value=\"direction_out direction_in direction_high_z\" /><attribute name=\"IEC_type\" filename=\"in_IEC_type\" value=\"Type_1_3\" /><attribute name=\"IEC_type_available\" filename=\"in_IEC_type_available\" value=\"Type_1_3 Type_2\" /><attribute name=\"enabled\" filename=\"in_voltage3_enabled\" value=\"0\" /></channel><attribute name=\"apply\" value=\"0\" /></device></context>";

	uint8_t eth_recv_data[100] = {0};
	uint8_t eth_data[4] = {0x99, 0x99, 0x99, 0x99};
	ret = eth_desc->noos_net.socket_listen(eth_desc, 0, 10);

	do {
		ret = eth_desc->noos_net.socket_accept(eth_desc, 0, &client_id);
	} while (ret == -EAGAIN);

	do {
		ret = eth_desc->noos_net.socket_recv(eth_desc, 1, eth_recv_data, 5);
	} while (!ret);

	char *ab = "PRINT21shfghfghfghfghdfghfdghdfghfdghdfghfdghfgytyryrtydfsfsfs2";
	ret = eth_desc->noos_net.socket_send(eth_desc, 1, demo_ctx, strlen(demo_ctx));
	no_os_mdelay(3000);
	while(1);

	// ret = adin1110_init(&adin1110, &adin1110_ip);
	// if (ret)
	// 	return ret;

	max14906_iio_ip.max14906_init_param = &max14906_ip;
	ad74413r_iio_ip.ad74413r_init_param = &ad74413r_ip;

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

	ret = no_os_irq_set_priority(ad74413r_irq_desc, ad74413r_gpio_irq_ip.irq_ctrl_id, 1);
	if (ret)
		return ret;

	// ret = no_os_irq_enable(ad74413r_irq_desc, ad74413r_irq_desc->irq_ctrl_id);
	// if (ret)
	// 	return ret;

	ad74413r_gpio_trig_ip.irq_ctrl = ad74413r_irq_desc;

	/* Initialize hardware trigger */
	ad74413r_gpio_trig_ip.iio_desc = &iio_desc,
	ret = iio_hw_trig_init(&ad74413r_trig_desc, &ad74413r_gpio_trig_ip);
	if (ret)
		return ret;

	/* Probe the iio drivers in config mode */
	ret = ad74413r_iio_init(&ad74413r_iio_desc, &ad74413r_iio_ip, true);
	if (ret)
		return ret;

	ret = max14906_iio_init(&max14906_iio_desc, &max14906_iio_ip, true);
	if (ret)
		return ret;

	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(AD74413R_GPIO_TRIG_NAME, ad74413r_trig_desc,
				&ad74413r_iio_trig_desc)
	};

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad74413r",
			.dev = ad74413r_iio_desc,
			.dev_descriptor = ad74413r_iio_desc->iio_dev,
			.read_buff = &buff,
		},
		{
			.name = "max14906",
			.dev = max14906_iio_desc,
			.dev_descriptor = max14906_iio_desc->iio_dev,
			.read_buff = &buff2,
		}
	};

	while (1) {
		ret = iio_app_run(iio_devices, NO_OS_ARRAY_SIZE(iio_devices));
		if (ret)
			return ret;

		/* Probe the drivers in the run mode */
		ret = max14906_iio_init(&max14906_iio_desc, &max14906_iio_ip, false);
		if (ret)
			return ret;

		ret = ad74413r_iio_init(&ad74413r_iio_desc, &ad74413r_iio_ip, false);
		if (ret)
			return ret;

		iio_devices[0].dev = ad74413r_iio_desc;
		iio_devices[0].dev_descriptor = ad74413r_iio_desc->iio_dev;

		iio_devices[1].dev = max14906_iio_desc;
		iio_devices[1].dev_descriptor = max14906_iio_desc->iio_dev;

		return iio_app_run_with_trigs(iio_devices, NO_OS_ARRAY_SIZE(iio_devices),
				      	      trigs, NO_OS_ARRAY_SIZE(trigs), ad74413r_irq_desc, &iio_desc);
	}
}
