#include <stdio.h>
#include <sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xparameters.h>
#include "xil_printf.h"
#include "spi_engine.h"
#include "ad400x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD400X_DMA_BASEADDR             XPAR_AXI_AD40XX_DMA_BASEADDR
#define AD400X_SPI_ENGINE_BASEADDR      XPAR_SPI_AD40XX_AXI_BASEADDR
#define AD400x_SPI_CS                   0

#define SPI_ENGINE_OFFLOAD_EXAMPLE	1

struct ad400x_init_param ad400x_init_param = {
	/* SPI engine*/
	{
		AD400X_SPI_ENGINE_BASEADDR,	/* adc_baseaddr */
		AD400x_SPI_CS,			/* chip_select */
		2,				/* cs_delay */
		0,				/* spi_config */
		83333333,			/* spi_clk_hz */
		2000000,			/* spi_clk_hz_reg_access */
		166666667,			/* ref_clk_hz */
		1,				/* spi_offload_rx_support_en */
		AD400X_DMA_BASEADDR,		/* spi_offload_rx_dma_baseaddr */
		1,				/* spi_offload_tx_support_en */
		AD400X_DMA_BASEADDR		/* spi_offload_tx_dma_baseaddr */
	},
	ID_AD4003, 20, /* dev_id, num_bits */
	1,0,0,0,
};

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void mdelay(uint32_t msecs)
{
	usleep(msecs * 1000);
}

int main()
{
	struct ad400x_dev *dev;
	uint32_t *offload_data;
	uint32_t adc_data;
	spi_eng_msg *msg;
	int32_t ret, data;
	uint32_t i;

	print("Test\n\r");

	uint32_t spi_eng_msg_cmds[3] = { CS_DEASSERT,
					 TRANSFER_BYTES_R(2),
					 CS_ASSERT
				       };

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = ad400x_init(&dev, ad400x_init_param);
	if (ret < 0)
		return ret;

	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0) {
		while(1) {
			ad400x_spi_single_conversion(dev, &adc_data);
			xil_printf("ADC: %d\n\r", adc_data);
		}
	}
	/* Offload example */
	else {
		msg = (spi_eng_msg *)malloc(sizeof(*msg));
		if (!msg)
			return -1;

		msg->spi_msg_cmds = spi_eng_msg_cmds;
		msg->msg_cmd_len = ARRAY_SIZE(spi_eng_msg_cmds);
		msg->rx_buf_addr = 0x800000;
		msg->tx_buf_addr = 0xA000000;
		msg->tx_buf[0] = 0xFF;
		msg->tx_buf[1] = 0xFF;

		/* Init the rx buffer with 0s */
		memset(msg->rx_buf, 0, 4 * ARRAY_SIZE(msg->rx_buf));

		spi_eng_offload_load_msg(dev->spi_desc, msg);
		spi_eng_transfer_multiple_msgs(dev->spi_desc, 1000);

		mdelay(2000);

		free(msg);

		offload_data = (uint32_t *)dev->spi_desc->rx_dma_startaddr;

		for(i = 0; i < dev->spi_desc->rx_length/4; i++) {
			data = *offload_data & 0xFFFFF;
			if (data > 524287)
				data = data - 1048576;
			printf("ADC%d: %d\n", i, data);
			offload_data += 1;
		}
	}

	spi_eng_write(dev->spi_desc, SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x0000);

	print("Success\n\r");

	Xil_DCacheDisable();
	Xil_ICacheDisable();
}
