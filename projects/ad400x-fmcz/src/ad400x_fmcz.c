#include <stdio.h>
#include <sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xparameters.h>
#include <inttypes.h>
#include "xil_printf.h"
#include "spi_engine.h"
#include "ad400x.h"
#include "no_os_error.h"
#include "axi_pwm_extra.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD400x_EVB_SAMPLE_NO			10000
#define AD400X_DMA_BASEADDR             XPAR_AXI_PULSAR_ADC_DMA_BASEADDR
#define AD400X_SPI_ENGINE_BASEADDR      XPAR_SPI_PULSAR_ADC_SPI_PULSAR_ADC_AXI_REGMAP_BASEADDR
#define AD400x_SPI_CS                   0
#define AD400x_SPI_ENG_REF_CLK_FREQ_HZ	XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ
#define AD400x_RX_CLKGEN_BASEADDR       XPAR_SPI_CLKGEN_BASEADDR
#define AXI_PWMGEN_BASEADDR             XPAR_PULSAR_ADC_TRIGGER_GEN_BASEADDR

#ifndef SPI_ENGINE_OFFLOAD_EXAMPLE
#define SPI_ENGINE_OFFLOAD_EXAMPLE	1
#endif

/**
 * Print data on the right format.
 * @param adc_data - the data to print.
 * @param res the resolution of the device.
 * @param sign - signed or unsigned.
 * @return - None.
 */
static void print_output_data(uint32_t adc_data, uint16_t res, char sign)
{
	uint32_t data = adc_data;

	if (sign == 's')
		xil_printf("ADC: %d\n\r", no_os_sign_extend32(data, res - 1));
	else
		xil_printf("ADC: %d\n\r", data);
}

int main()
{
	struct ad400x_dev *dev;
	uint32_t *offload_data;
	uint32_t adc_data;
	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = OFFLOAD_RX_EN,
		.rx_dma_baseaddr = AD400X_DMA_BASEADDR,
	};
	struct spi_engine_offload_message msg;
	uint32_t commands_data[2] = {0xFF, 0xFF};
	int32_t ret, i;
	enum ad400x_supported_dev_ids dev_id = ID_AD4020;
	uint16_t res = ad400x_device_resol[dev_id];

	struct spi_engine_init_param spi_eng_init_param  = {
		.ref_clk_hz = 160000000,
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD400X_SPI_ENGINE_BASEADDR,
		.cs_delay = 1,
		.data_width = res,
	};

	struct axi_clkgen_init clkgen_init = {
		.name = "rx_clkgen",
		.base = AD400x_RX_CLKGEN_BASEADDR,
		.parent_rate = 100000000,
	};

	struct axi_pwm_init_param axi_pwm_init = {
		.base_addr = AXI_PWMGEN_BASEADDR,
		.ref_clock_Hz = 160000000,
		.channel = 0,
	};

	struct no_os_pwm_init_param trigger_pwm_init = {
		.period_ns = 555,
		.duty_cycle_ns = 10,
		.polarity = NO_OS_PWM_POLARITY_HIGH,
		.platform_ops = &axi_pwm_ops,
		.extra = &axi_pwm_init,
	};

	struct ad400x_init_param ad400x_init_param = {
		.spi_init = {
			.chip_select = AD400x_SPI_CS,
			.max_speed_hz = 80000000,
			.mode = NO_OS_SPI_MODE_0,
			.platform_ops = &spi_eng_platform_ops,
			.extra = (void*)&spi_eng_init_param,
		},
		.trigger_pwm_init = &trigger_pwm_init,
		.clkgen_init = &clkgen_init,
		.axi_clkgen_rate = 160000000,
		.reg_access_speed = 1000000,
		.dev_id = dev_id,
		.turbo_mode = 1,
		.high_z_mode = 0,
		.span_compression = 0,
		.en_status_bits = 0,
	};

	print("Test\n\r");

	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		READ(2),
		CS_HIGH
	};

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	/* data must be byte alligned when offload is disabled */
	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0)
		spi_eng_init_param.data_width =
			NO_OS_DIV_ROUND_UP(spi_eng_init_param.data_width, 8) * 8;

	ret = ad400x_init(&dev, &ad400x_init_param);
	if (ret < 0)
		return ret;

	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0) {
		while(1) {
			ad400x_spi_single_conversion(dev, &adc_data);
			print_output_data(adc_data, res, ad400x_device_sign[dev_id]);
		}
	}
	/* Offload example */
	else {
		ret = spi_engine_offload_init(dev->spi_desc, &spi_engine_offload_init_param);
		if (ret != 0)
			return -1;

		msg.commands = spi_eng_msg_cmds;
		msg.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
		msg.rx_addr = 0x800000;
		msg.tx_addr = 0xA000000;
		msg.commands_data = commands_data;

		ret = spi_engine_offload_transfer(dev->spi_desc, msg, AD400x_EVB_SAMPLE_NO * 2);
		if (ret != 0)
			return ret;

		no_os_mdelay(2000);
		Xil_DCacheInvalidateRange(0x800000, AD400x_EVB_SAMPLE_NO * 4);
		offload_data = (uint32_t *)msg.rx_addr;

		for(i = 0; i < AD400x_EVB_SAMPLE_NO; i++) {
			print_output_data(*offload_data, res, ad400x_device_sign[dev_id]);
			offload_data += 1;
		}
	}

	print("Success\n\r");

	Xil_DCacheDisable();
	Xil_ICacheDisable();
}
