/***************************************************************************//**
 *   @file   ad9467_fmc.c
 *   @brief  Implementation of the program's main function.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include "xil_cache.h"
#include "xparameters.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_delay.h"
#include "no_os_error.h"

#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "no_os_axi_io.h"
#include "ad9467.h"
#include "ad9517.h"
#include "parameters.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "xilinx_uart.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* ADC registers */
#define CF_REG_DELAY_CTRL			0x60
#define CF_REG_DELAY_STATUS			0x20
#define CF_REG_DATA_MONITOR			0x404

/* CF_REG_DATA_MONITOR bit definition. */
#define CF_DATA_MONITOR_PN_ERR		(1 << 2) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_SYNC		(1 << 1) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_OVER_RNG	(1 << 0) // (Write 1 to clear)

enum type_output_modes {
	OFFSET_BINARY = 0,
	TWOS_COMPLEMENT,
	GRAY_CODE,
} type_output_modes;

enum type_test_modes {
	TEST_DISABLE = 0,
	MIDSCALE,
	POS_FULLSCALE,
	NEG_FULLSCALE,
	CHECKERBOARD,
	PN_23_SEQUENCE,
	PN_9_SEQUENCE,
	ONE_ZERO_TOGGLE
} type_test_modes;

struct ad9517_platform_data ad9517_pdata_lpc = {
	/* PLL Reference */
	250000000, // ref_1_freq
	250000000, // ref_2_freq
	1, // diff_ref_en
	1, // ref_1_power_on
	1, // ref_2_power_on
	0, // ref_sel_pin_en
	1, // ref_sel_pin
	0, // ref_2_en

	250000000, // ext_clk_freq
	1600000000, // int_vco_freq
	0, // vco_clk_sel
	0, // power_down_vco_clk
	"ad9517-lpc" // name[16]
};

struct ad9517_lvpecl_channel_spec ad9517_lvpecl_channels[] = {
	{
		0, // channel_num - Output channel number.
		0, // out_invert_en - Invert the polarity of the output clock.
		LVPECL_780mV, // out_diff_voltage - LVPECL output differential voltage.
		"CH0" // name[16] - Optional descriptive channel name.
	},
	{
		1, // channel_num - Output channel number.
		0, // out_invert_en - Invert the polarity of the output clock.
		LVPECL_780mV, // out_diff_voltage - LVPECL output differential voltage.
		"CH1" // name[16] - Optional descriptive channel name.
	},
	{
		2, // channel_num - Output channel number.
		0, // out_invert_en - Invert the polarity of the output clock.
		LVPECL_780mV, // out_diff_voltage - LVPECL output differential voltage.
		"CH2" // name[16] - Optional descriptive channel name.
	},
	{
		3, // channel_num - Output channel number.
		0, // out_invert_en - Invert the polarity of the output clock.
		LVPECL_960mV, // out_diff_voltage - LVPECL output differential voltage.
		"CH3" // name[16] - Optional descriptive channel name.
	}
};

struct ad9517_lvds_cmos_channel_spec ad9517_lvds_cmos_channels[] = {
	{
		4, // channel_num - Output channel number.
		0, // out_invert
		LVDS, // logic_level - Select LVDS or CMOS logic levels.
		0, // cmos_b_en - In CMOS mode, turn on/off the CMOS B output.
		LVDS_3_5mA, // out_lvds_current - LVDS output current level.
		"CH4" // name[16] - Optional descriptive channel name.
	},
	{
		5, // channel_num - Output channel number.
		0, // out_invert
		LVDS, // logic_level - Select LVDS or CMOS logic levels.
		0, // cmos_b_en - In CMOS mode, turn on/off the CMOS B output.
		LVDS_3_5mA, // out_lvds_current - LVDS output current level.
		"CH5" // name[16] - Optional descriptive channel name.
	},
	{
		6, // channel_num - Output channel number.
		0, // out_invert
		LVDS, // logic_level - Select LVDS or CMOS logic levels.
		1, // cmos_b_en - In CMOS mode, turn on/off the CMOS B output.
		LVDS_3_5mA, // out_lvds_current - LVDS output current level.
		"CH6" // name[16] - Optional descriptive channel name.
	},
	{
		7, // channel_num - Output channel number.
		0, // out_invert
		LVDS, // logic_level - Select LVDS or CMOS logic levels.
		0, // cmos_b_en - In CMOS mode, turn on/off the CMOS B output.
		LVDS_3_5mA, // out_lvds_current - LVDS output current level.
		"CH7" // name[16] - Optional descriptive channel name.
	}
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
void adc_test(struct axi_adc *adc,
	      struct axi_dmac *dma,
	      struct ad9467_dev *dev,
	      uint32_t mode,
	      uint32_t format);

void display_test_mode(uint32_t mode, uint32_t format);

/***************************************************************************//**
 * @brief Main function.
 *
 * @return 0 in case of success, -1 otherwise.
*******************************************************************************/
int main()
{

	uint32_t mode;
	uint8_t ret_val;
	uint32_t ret_val_32 = 0;
	int32_t status, ret;
	uint8_t i;

	Xil_DCacheDisable();
	Xil_ICacheEnable();

	// SPI configuration
	struct no_os_spi_init_param ad9467_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.platform_ops = &xil_spi_ops,
		.mode = NO_OS_SPI_MODE_0
	};

	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};
	ad9467_spi_param.device_id = SPI_DEVICE_ID;
	ad9467_spi_param.extra = &xil_spi_param;

	struct no_os_spi_init_param ad9517_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000u,
		.chip_select = 1,
		.platform_ops = &xil_spi_ops,
		.mode = NO_OS_SPI_MODE_0
	};
	ad9517_spi_param.extra = &xil_spi_param;

	struct ad9467_init_param ad9467_init = {
		.spi_init = ad9467_spi_param
	};
	struct ad9467_dev *ad9467_device;

	struct ad9517_state ad9517_state_init = {
		.pdata = &ad9517_pdata_lpc,
		.lvpecl_channels = ad9517_lvpecl_channels,
		.lvds_cmos_channels = ad9517_lvds_cmos_channels
	};

	struct ad9517_init_param ad9517_init = {
		.spi_init = ad9517_spi_param,
		.ad9517_type = AD9517_4,
		.ad9517_st = ad9517_state_init
	};
	struct ad9517_dev *ad9517_device;

	struct axi_adc_init ad9467_core_param = {
		.name = "ad9467_core",
		.num_channels = 1,
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *ad9467_core;

	struct axi_dmac_init ad9467_dmac_param = {
		.name = "ad9625_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};
	struct axi_dmac *ad9467_dmac;

	/* AD9467 Setup. */
	status = ad9467_setup(&ad9467_device, ad9467_init);
	if (status)
		return status;

	/* AD9517 Setup. */
	/* Initialize device. */
	status = ad9517_setup(&ad9517_device, ad9517_init);
	if (status)
		return status;
	/* Set channel 3 for normal operation */
	status = ad9517_power_mode(ad9517_device, 3, 0);
	if (status)
		return status;
	/* Set the channel 3 frequency to 250Mhz */
	ad9517_frequency(ad9517_device, 3, 250000000);
	/* Update registers */
	status = ad9517_update(ad9517_device);
	if (status)
		return status;

	/* Read the device ID for AD9467 and AD9517. */
	printf("\n\r*****************************************************\r\n");
	printf("  ADI AD9467-FMC-EBZ Reference Design\n\r");
	status = ad9467_read(ad9467_device, AD9467_REG_CHIP_ID, &ret_val);
	if (status)
		return status;
	printf("  AD9467 CHIP ID: 0x%02x\n\r", ret_val);
	status = ad9467_read(ad9467_device, AD9467_REG_CHIP_GRADE, &ret_val);
	if (status)
		return status;
	printf("  AD9467 CHIP GRADE: 0x%02x\n\r", ret_val);
	status = ad9517_read(ad9517_device, AD9517_REG_PART_ID, &ret_val_32);
	if (status)
		return status;
	printf("  AD9517 CHIP ID: 0x%02x", ret_val_32);
	printf("\n\r*****************************************************\r\n");

	status = axi_adc_init(&ad9467_core,  &ad9467_core_param);
	if (status != 0) {
		printf("axi_adc_init() error: %s\n", ad9467_core->name);
		return status;
	}

	status = axi_dmac_init(&ad9467_dmac, &ad9467_dmac_param);
	if (status != 0) {
		printf("axi_dmac_init() error: %s\n", ad9467_dmac->name);
		return status;
	}

	// setup device
	status = ad9467_write(ad9467_device, AD9467_REG_TEST_IO, 0x05); // pn23
	if (status)
		return status;
	status = ad9467_write(ad9467_device, AD9467_REG_DEVICE_UPDATE, 0x01); // update
	if (status)
		return status;
	status = ad9467_write(ad9467_device, AD9467_REG_DEVICE_UPDATE, 0x00);
	if (status)
		return status;

	status = ad9467_read(ad9467_device, AD9467_REG_OUT_PHASE, &ret_val);
	if (status)
		return status;
	printf("AD9467[0x016]: %02x\n\r", ret_val);
	// setup adc core

	status = axi_adc_write(ad9467_core, AXI_ADC_REG_CNTRL, 0x2);
	if (status)
		return status;
	for(i = 0; i < ad9467_core->num_channels; i++) {
		status = axi_adc_write(ad9467_core, AXI_ADC_REG_CHAN_CNTRL(i), 0x03);
		if (status)
			return status;
	}

	status = axi_adc_write(ad9467_core, AXI_ADC_REG_DELAY_CNTRL, 0x0);
	if (status)
		return status;
	status = axi_adc_write(ad9467_core, AXI_ADC_REG_DELAY_CNTRL, 0x20F1F);
	if (status)
		return status;

	no_os_mdelay(10);
	if (axi_adc_delay_calibrate(ad9467_core, 8, 1)) {
		status = ad9467_read(ad9467_device, 0x16, &ret_val);
		if (status)
			return status;
		printf("AD9467[0x016]: %02x\n\r", ret_val);
		status = ad9467_write(ad9467_device, AD9467_REG_OUT_PHASE, 0x80);
		if (status)
			return status;
		status = ad9467_write(ad9467_device, AD9467_REG_DEVICE_UPDATE, 0x01);
		if (status)
			return status;
		status = ad9467_write(ad9467_device, AD9467_REG_DEVICE_UPDATE, 0x00);
		if (status)
			return status;
		status = ad9467_read(ad9467_device, 0x16, &ret_val);
		if (status)
			return status;
		printf("AD9467[0x016]: %02x\n\r", ret_val);
		no_os_mdelay(10);
		if (axi_adc_delay_calibrate(ad9467_core, 16, 1)) {
			printf("adc_setup: can not set a zero error delay!\n\r");
		}
	}

	/* Data pattern checks */
	for (mode = MIDSCALE; mode <= ONE_ZERO_TOGGLE; mode++) {
		/* Data format is offset binary */
		adc_test(ad9467_core, ad9467_dmac, ad9467_device, mode, OFFSET_BINARY);
		/* Data format is twos complement */
		adc_test(ad9467_core, ad9467_dmac, ad9467_device, mode, TWOS_COMPLEMENT);
	}
	printf("Testing done.\n\r");
	/* AD9467 Setup for data acquisition */
	ret = ad9467_output_invert(ad9467_device, 0, &status);	// Output invert Off
	if (ret)
		return ret;
	ret = ad9467_transfer(ad9467_device);				// Synchronously update registers
	if (ret)
		return ret;
	ret = ad9467_output_format(ad9467_device, 1, &status);	// Twos complement
	if (ret)
		return ret;
	ret = ad9467_transfer(ad9467_device);				// Synchronously update registers
	if (ret)
		return ret;
	ret = ad9467_reset_pn9(ad9467_device, 0, &status);		// Clear PN9 bit
	if (ret)
		return ret;
	ret = ad9467_transfer(ad9467_device);				// Synchronously update registers
	if (ret)
		return ret;
	ret = ad9467_reset_pn23(ad9467_device, 0, &status);		// Clear PN23 bit
	if (ret)
		return ret;
	ret = ad9467_transfer(ad9467_device);				// Synchronously update registers
	if (ret)
		return ret;
	ret = ad9467_test_mode(ad9467_device, 0, &status);		// Test mode Off
	if (ret)
		return ret;
	ret = ad9467_transfer(ad9467_device);				// Synchronously update registers
	if (ret)
		return ret;

	printf("Start capturing data...\n\r");

	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = 16384 * 2,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	status = axi_dmac_transfer_start(ad9467_dmac, &read_transfer);
	if(status)
		return status;
	/* Wait until transfer finishes */
	status = axi_dmac_transfer_wait_completion(ad9467_dmac, 500);
	if(status)
		return status;
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR, 16384 * 2);

#ifdef IIO_SUPPORT
	struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#endif
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *dev_desc;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9467_core,
		.rx_dmac = ad9467_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};
	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (NO_OS_IS_ERR_VALUE(status))
		return status;

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);

	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad9467", iio_axi_adc_desc, dev_desc, &read_buff,
			       NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
#endif

	printf("Done.\n\r");

	ad9467_remove(ad9467_device);
	ad9517_remove(ad9517_device);

	Xil_DCacheEnable();
	Xil_ICacheDisable();

	return 0;
}

void adc_test(struct axi_adc *adc,
	      struct axi_dmac *dmac,
	      struct ad9467_dev *dev,
	      uint32_t mode,
	      uint32_t format)
{
	uint32_t n = 0;
	uint32_t rdata;
	uint32_t edata = 0;
	uint32_t error = 0;
	int32_t read;
	int32_t status = -1;

	ad9467_output_format(dev, format, &read);
	ad9467_test_mode(dev, mode, &read);
	ad9467_transfer(dev);
	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = 16384,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	axi_dmac_transfer_start(dmac, &read_transfer);
	/* Wait until transfer finishes */
	status = axi_dmac_transfer_wait_completion(dmac, 500);
	if(status)
		printf("  ERROR: DMA transfer.\n\r");
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR, 16384);

	display_test_mode(mode, format);
	if ((mode == PN_23_SEQUENCE) || (mode == PN_9_SEQUENCE)) {
		if (format == TWOS_COMPLEMENT) {
			printf("          Test skipped\r\n");
			return;
		}

		axi_adc_set_pnsel(adc, 0,
				  ((mode == PN_23_SEQUENCE) ? AXI_ADC_PN23A : AXI_ADC_PN9));
		no_os_mdelay(10);
		axi_adc_write(adc,
			      CF_REG_DATA_MONITOR,
			      CF_DATA_MONITOR_PN_ERR |
			      CF_DATA_MONITOR_PN_SYNC |
			      CF_DATA_MONITOR_PN_OVER_RNG); // write ones to clear bits
		no_os_mdelay(100);
		axi_adc_read(adc, CF_REG_DATA_MONITOR, &rdata);
		if ((rdata & (CF_DATA_MONITOR_PN_ERR |
			      CF_DATA_MONITOR_PN_SYNC |
			      CF_DATA_MONITOR_PN_OVER_RNG)) != 0) {
			axi_adc_read(adc, CF_REG_DATA_MONITOR, &rdata);
			printf("  ERROR: PN status(%04x).\n\r", rdata);
		} else {
			printf("          Test passed\r\n");
		}
		return;
	}

	for (n = 0; n < 32; n++) {
		no_os_axi_io_read(ADC_DDR_BASEADDR, n*4, &rdata);
		if ((mode == MIDSCALE) && (format == OFFSET_BINARY))
			edata = 0x80008000;
		if ((mode == POS_FULLSCALE) && (format == OFFSET_BINARY))
			edata = 0xffffffff;
		if ((mode == NEG_FULLSCALE) && (format == OFFSET_BINARY))
			edata = 0x00000000;
		if ((mode == MIDSCALE) && (format == TWOS_COMPLEMENT))
			edata = 0x00000000;
		if ((mode == POS_FULLSCALE) && (format == TWOS_COMPLEMENT))
			edata = 0x7fff7fff;
		if ((mode == NEG_FULLSCALE) && (format == TWOS_COMPLEMENT))
			edata = 0x80008000;
		if (((mode == CHECKERBOARD) || (mode == ONE_ZERO_TOGGLE)) && (n == 0)) {
			edata = (rdata & 0xffff);
		}
		if ((mode == CHECKERBOARD) && (n == 0))
			edata = (edata == 0xaaaa) ? 0x5555aaaa : 0xaaaa5555;
		if ((mode == ONE_ZERO_TOGGLE) && (n == 0))
			edata = (edata == 0xffff) ? 0x0000ffff : 0xffff0000;
		if (rdata != edata) {
			printf("  ERROR[%2u]: rcv(%08x), exp(%08x)\n\r", n, rdata,
			       edata);
			error = 1;
		}
	}
	if(error == 0) {
		printf("		Test passed\r\n");
	}
}

void display_test_mode(uint32_t mode, uint32_t format)
{
	char *s_mode;
	char *s_format;

	switch(format) {
	case OFFSET_BINARY:
		s_format = "OFFSET BINARY";
		break;
	case TWOS_COMPLEMENT:
		s_format = "TWOS_COMPLEMENT";
		break;
	case GRAY_CODE:
		s_format = "GRAY_CODE";
		break;
	default:
		s_format = "";
		break;
	}
	switch(mode) {
	case TEST_DISABLE:
		s_mode = "TEST_DISABLE BINARY";
		break;
	case MIDSCALE:
		s_mode = "MIDSCALE";
		break;
	case POS_FULLSCALE:
		s_mode = "POS_FULLSCALE";
		break;
	case NEG_FULLSCALE:
		s_mode = "NEG_FULLSCALE BINARY";
		break;
	case CHECKERBOARD:
		s_mode = "CHECKERBOARD";
		break;
	case PN_23_SEQUENCE:
		s_mode = "PN_23_SEQUENCE";
		break;
	case PN_9_SEQUENCE:
		s_mode = "PN_9_SEQUENCE";
		break;
	case ONE_ZERO_TOGGLE:
		s_mode = "ONE_ZERO_TOGGLE";
		break;
	default:
		s_mode = "";
		break;
	}
	printf("ADC Test: mode - %s\r\n          format - %s\n\r",
	       s_mode,
	       s_format);
}
