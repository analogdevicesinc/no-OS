/***************************************************************************//**
 *   @file   iio_lwip_example.c
 *   @brief  IIO over Ethernet (lwIP / CAPI) example for Xilinx Zynq boards.
 *
 *   Exposes virtual ADC and DAC demo devices over the iiod TCP protocol
 *   on port 30431. Connect from a host running libiio:
 *
 *     iio_info -n ip:<board-ip>
 *
 *   @author Nicolae-Daniel Deaconescu (Nicolae-daniel.Deaconescu@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <string.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_app.h"
#include "lwip_capi.h"
#include "capi_xemacps.h"
#include "adc_demo.h"
#include "iio_adc_demo.h"
#include "dac_demo.h"
#include "iio_dac_demo.h"
#include "common_data.h"
#include "parameters.h"
#include "iio_lwip_example.h"

#if PHY_TYPE == PHY_TYPE_TI_DP83867
#include "capi_dp83867.h"
#else
#include "capi_marvell_88e1510.h"
#endif

/*
 * MAC address for this board. Must be unique on the local network.
 * Change the last three bytes (OUI 00:0A:35 is assigned to Xilinx/AMD).
 */
static uint8_t mac_addr[6] = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02};

/* IIO data buffers */
#define DATA_BUFFER_SAMPLES	SAMPLES_PER_CHANNEL_PLATFORM
static uint8_t adc_data_buffer[DATA_BUFFER_SAMPLES * TOTAL_ADC_CHANNELS *
						   sizeof(uint16_t)];
static uint8_t dac_data_buffer[DATA_BUFFER_SAMPLES * TOTAL_DAC_CHANNELS *
						   sizeof(uint16_t)];

/* MDIO trampolines bound to the example's MAC handle. */
static struct capi_eth_mac_handle *g_mac;

static int example_mdio_read(uint8_t addr, uint8_t reg, uint16_t *data)
{
	return xemacps_mdio_read(g_mac, addr, reg, data);
}

static int example_mdio_write(uint8_t addr, uint8_t reg, uint16_t data)
{
	return xemacps_mdio_write(g_mac, addr, reg, data);
}

/***************************************************************************//**
 * @brief Run the IIO over Ethernet demo.
 *
 * Initialises the virtual ADC/DAC IIO demo devices, configures the
 * XEmacPs MAC + Marvell PHY pair via the generic CAPI lwIP glue, and
 * enters the iio_app_run() loop. This function does not return under
 * normal operation.
 *
 * @return 0 on success, negative error code on initialisation failure.
*******************************************************************************/
int iio_lwip_example_main(void)
{
	struct adc_demo_desc *adc_desc;
	struct dac_demo_desc *dac_desc;
	struct iio_app_desc *app;
	int ret;

	/* --- Initialise virtual IIO devices --- */
	ret = adc_demo_init(&adc_desc, &adc_init_par);
	if (ret)
		return ret;

	ret = dac_demo_init(&dac_desc, &dac_init_par);
	if (ret)
		goto remove_adc;

	/* --- IIO device + buffer descriptors --- */
	struct iio_data_buffer adc_buff = {
		.buff = adc_data_buffer,
		.size = sizeof(adc_data_buffer),
	};

	struct iio_data_buffer dac_buff = {
		.buff = dac_data_buffer,
		.size = sizeof(dac_data_buffer),
	};

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("adc_demo", adc_desc,
			       &adc_demo_iio_descriptor, &adc_buff, NULL, NULL),
		IIO_APP_DEVICE("dac_demo", dac_desc,
			       &dac_demo_iio_descriptor, NULL, &dac_buff, NULL),
	};

	/* --- MAC + PHY configuration --- */
#if PHY_TYPE == PHY_TYPE_TI_DP83867
	struct dp83867_extra_config phy_cfg = {
		.rgmii = {
			.rx_delay_en   = true,
			.tx_delay_en   = true,
			.rx_delay_code = DP83867_RGMII_DELAY_2_00_NS,
			.tx_delay_code = DP83867_RGMII_DELAY_2_00_NS,
		},
	};
#else
	struct mrvl_88e1510_extra_config phy_cfg = {
		.rgmii = { .rx_delay_en = true, .tx_delay_en = true },
		.downshift_en = true,
		.downshift_retries = 3,
	};
#endif
	struct capi_eth_mac_init_config mac_cfg = {
		.identifier  = GEM_DEVICE_ID,
		.mac_address = (capi_eth_mac_addr *)mac_addr,
		.ops         = &xemacps_capi_mac_ops,
	};
	static struct lwip_capi_param netdev_param = {
		.phy_addr      = 0,                /* auto-detect */
		.phy_interface = CAPI_ETH_INTERFACE_RGMII,
		.phy_mode      = {
			.speed          = CAPI_ETH_PHY_SPEED_1G,
			.duplex         = CAPI_ETH_PHY_DUPLEX_FULL,
			.auto_negotiate = true,
			.mdix           = CAPI_ETH_MDIX_AUTO,
		},
	};
#if PHY_TYPE == PHY_TYPE_TI_DP83867
	netdev_param.phy_ops   = &dp83867_ops;
#else
	netdev_param.phy_ops   = &mrvl_88e1510_ops;
#endif
	netdev_param.phy_extra = &phy_cfg;
	netdev_param.fn_read   = example_mdio_read;
	netdev_param.fn_write  = example_mdio_write;
	memcpy(netdev_param.hwaddr, mac_addr, sizeof(mac_addr));

	ret = capi_eth_mac_init(&g_mac, &mac_cfg);
	if (ret)
		goto remove_dac;
	netdev_param.mac = g_mac;

	/* --- iio_app init --- */
	struct iio_app_init_param app_init_param = {0};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = xilinx_lwip_uart_ip;
	app_init_param.lwip_param.platform_ops = &lwip_capi_ops;
	app_init_param.lwip_param.mac_param    = &netdev_param;
	memcpy(app_init_param.lwip_param.hwaddr, mac_addr, sizeof(mac_addr));

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_mac;

	/* Loops indefinitely, serving iiod connections on TCP port 30431 */
	ret = iio_app_run(app);

	iio_app_remove(app);

remove_mac:
	capi_eth_mac_deinit(g_mac);
remove_dac:
	dac_demo_remove(dac_desc);
remove_adc:
	adc_demo_remove(adc_desc);

	return ret;
}
