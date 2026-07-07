/***************************************************************************//**
 *   @file   iiod_example.c
 *   @brief  IIOD example for K project: control + data plane over Ethernet.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright (C) 2026 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#include <string.h>
#include "iiod_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"

#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"

#include "axi_dmac.h"
#include "tx_generator.h"
#include "axi_adc_core.h"

#include "k_data.h"
#include "iio_k_tx.h"
#include "iio_k_rx.h"
#include "iio_k_regmap.h"
#include "iio_k_hdlregs.h"
#include "iio_k_gpio.h"
#include "iio_k_time.h"

#include "iio_app.h"
#include "lwip_xemacps.h"

/**
 * @struct k_hw
 * @brief  Handles produced by the hardware bring-up, consumed by k_data.
 */
struct k_hw {
	struct axi_dmac		*tx_dmac;
	struct axi_dmac		*rx_dmac;
	struct axi_dmac		*tx_cmd_dmac;
	struct axi_dmac		*rx_cmd_dmac;
	struct tx_generator_dev	*tx_generator;
	struct axi_adc		*rx_adc;
	/* JESD/adxcvr handles kept for teardown + k-hdl-regs window bases */
};

/*
 * TODO(phase 0): lift the Si5391 / adxcvr / JESD TX-RX / TPL / ad9081k / FSM
 * bring-up currently under `#if 0` in basic_example.c into this helper. For the
 * skeleton it is a stub that must be filled before the data plane works.
 */
static int k_hw_bringup(struct k_hw *hw)
{
	if (!hw)
		return -EINVAL;

	memset(hw, 0, sizeof(*hw));

	/* TODO: create Si5391, adxcvr(tx/rx), jesd(tx/rx), tx_generator,
	 * rx_adc, 4x axi_dmac, ad9081k; run jesd204_fsm_start(). Populate hw. */

	return -ENOSYS;
}

int example_main(void)
{
	struct k_hw hw;
	struct k_data *kd = NULL;
	int ret;

	/* IIO device descriptors */
	struct iio_k_tx_desc		*k_tx = NULL;
	struct iio_k_rx_desc		*k_rx = NULL;
	struct iio_k_regmap_desc	*k_regmap = NULL;
	struct iio_k_hdlregs_desc	*k_hdlregs = NULL;
	struct iio_k_gpio_desc		*k_gpio = NULL;
	struct iio_k_time_desc		*k_time = NULL;

	struct iio_device *tx_dev, *rx_dev, *regmap_dev, *hdlregs_dev,
			  *gpio_dev, *time_dev;

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	/* Ethernet transport (GEM) parameters. */
	static uint8_t mac_addr[6] = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02};
	struct xemacps_init_param gem_ip = {
		.device_id = GEM_DEVICE_ID,
	};
	memcpy(gem_ip.hwaddr, mac_addr, sizeof(mac_addr));

	pr_info("K IIOD example starting.\n");

	/* ---- 1. Hardware bring-up (JESD link, TPLs, DMAs) ---- */
	ret = k_hw_bringup(&hw);
	if (ret) {
		pr_err("k_hw_bringup() failed: %d\n", ret);
		return ret;
	}

	/* ---- 2. Data-plane engine ---- */
	struct k_data_init_param kd_ip = {
		.tx_dmac	= hw.tx_dmac,
		.rx_dmac	= hw.rx_dmac,
		.tx_cmd_dmac	= hw.tx_cmd_dmac,
		.rx_cmd_dmac	= hw.rx_cmd_dmac,
		.tx_generator	= hw.tx_generator,
		.rx_adc		= hw.rx_adc,
		.tx_offload_base	= TX_DATA_OFFLOAD_BASEADDR,
		.rx_offload_base	= RX_DATA_OFFLOAD_BASEADDR,
		.tx_ddr_base		= TX_DDR_BASEADDR,
		.rx_ddr_base		= RX_DDR_BASEADDR,
		.tx_cmd_ddr_base	= TX_CMD_DDR_BASEADDR,
		.rx_cmd_ddr_base	= RX_CMD_DDR_BASEADDR,
		.tx_ddr_size		= 0x100000,	/* 1 MiB staging window */
		.rx_ddr_size		= 0x100000,
		.num_channels		= ADC_CHANNELS,
		.dma_beat_bytes		= ADC_CHANNELS * sizeof(uint64_t),
	};

	ret = k_data_init(&kd, &kd_ip);
	if (ret) {
		pr_err("k_data_init() failed: %d\n", ret);
		return ret;
	}

	/* ---- 3. IIO devices ---- */
	struct iio_k_tx_init_param tx_ip = {
		.kd = kd,
		.stream_id = K_STREAM_PASS,
		.tx_mode = K_TX_ONESHOT,
		.payload_mode = K_PAYLOAD_SAMPLES,
		.timeout_ms = 2000,
	};
	ret = iio_k_tx_init(&k_tx, &tx_ip);
	if (ret)
		goto err_kd;
	iio_k_tx_get_dev_descriptor(k_tx, &tx_dev);

	struct iio_k_rx_init_param rx_ip = {
		.kd = kd,
		.payload_mode = K_PAYLOAD_SAMPLES,
		.timeout_ms = 2000,
	};
	ret = iio_k_rx_init(&k_rx, &rx_ip);
	if (ret)
		goto err_tx;
	iio_k_rx_get_dev_descriptor(k_rx, &rx_dev);

	struct iio_k_regmap_init_param regmap_ip = { .kd = kd };
	ret = iio_k_regmap_init(&k_regmap, &regmap_ip);
	if (ret)
		goto err_rx;
	iio_k_regmap_get_dev_descriptor(k_regmap, &regmap_dev);

	struct iio_k_hdlregs_init_param hdlregs_ip = {
		.base = {
			[K_HDL_WIN_JESD_TX]	= TX_JESD_BASEADDR,
			[K_HDL_WIN_JESD_RX]	= RX_JESD_BASEADDR,
			[K_HDL_WIN_ADXCVR_TX]	= TX_XCVR_BASEADDR,
			[K_HDL_WIN_ADXCVR_RX]	= RX_XCVR_BASEADDR,
			[K_HDL_WIN_TPL_TX]	= TX_CORE_BASEADDR,
			[K_HDL_WIN_TPL_RX]	= RX_CORE_BASEADDR,
			[K_HDL_WIN_OFFLOAD_TX]	= TX_DATA_OFFLOAD_BASEADDR,
			[K_HDL_WIN_OFFLOAD_RX]	= RX_DATA_OFFLOAD_BASEADDR,
		},
	};
	ret = iio_k_hdlregs_init(&k_hdlregs, &hdlregs_ip);
	if (ret)
		goto err_regmap;
	iio_k_hdlregs_get_dev_descriptor(k_hdlregs, &hdlregs_dev);

	/* GPIO: expose the chip reset line for now. */
	static struct no_os_gpio_init_param rstb_param = {
		.number = AD9081K_RESET_B,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param,
	};
	struct iio_k_gpio_init_param gpio_ip = {
		.num_lines = 1,
		.lines = {
			{ .name = "reset_b", .init_param = &rstb_param },
		},
	};
	ret = iio_k_gpio_init(&k_gpio, &gpio_ip);
	if (ret)
		goto err_hdlregs;
	iio_k_gpio_get_dev_descriptor(k_gpio, &gpio_dev);

	struct iio_k_time_init_param time_ip = { .kd = kd };
	ret = iio_k_time_init(&k_time, &time_ip);
	if (ret)
		goto err_gpio;
	iio_k_time_get_dev_descriptor(k_time, &time_dev);

	/* ---- 4. Register devices with the IIO application ---- */
	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("k-tx", k_tx, tx_dev, NULL, NULL, NULL),
		IIO_APP_DEVICE("k-rx", k_rx, rx_dev, NULL, NULL, NULL),
		IIO_APP_DEVICE("k-regmap", k_regmap, regmap_dev, NULL, NULL, NULL),
		IIO_APP_DEVICE("k-hdl-regs", k_hdlregs, hdlregs_dev, NULL, NULL, NULL),
		IIO_APP_DEVICE("k-gpio", k_gpio, gpio_dev, NULL, NULL, NULL),
		IIO_APP_DEVICE("k-time", k_time, time_dev, NULL, NULL, NULL),
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = xilinx_lwip_uart_ip;
	app_init_param.lwip_param.platform_ops = &xemacps_lwip_ops;
	app_init_param.lwip_param.mac_param = &gem_ip;
	memcpy(app_init_param.lwip_param.hwaddr, mac_addr, sizeof(mac_addr));

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("iio_app_init() failed: %d\n", ret);
		goto err_time;
	}

	/* ---- 5. Serve forever (Ethernet transport). ---- */
	return iio_app_run(app);

err_time:
	iio_k_time_remove(k_time);
err_gpio:
	iio_k_gpio_remove(k_gpio);
err_hdlregs:
	iio_k_hdlregs_remove(k_hdlregs);
err_regmap:
	iio_k_regmap_remove(k_regmap);
err_rx:
	iio_k_rx_remove(k_rx);
err_tx:
	iio_k_tx_remove(k_tx);
err_kd:
	k_data_remove(kd);

	return ret;
}
