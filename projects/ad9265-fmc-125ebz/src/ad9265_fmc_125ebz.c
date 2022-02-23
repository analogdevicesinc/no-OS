
#include "xil_cache.h"
#include "xparameters.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "ad9265.h"
#include "no-os/spi.h"
#include "no-os/gpio.h"
#include "spi_extra.h"
#include "parameters.h"
#include "no-os/error.h"
#include "gpio_extra.h"

#include "no-os/print_log.h"


static int32_t adaq8092_if_gpio_setup(uint32_t gpio_no, uint8_t gpio_val)
{
	struct xil_gpio_init_param ps_gpio_init = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};
	struct gpio_init_param temp_init = {
		.number = gpio_no,
		.platform_ops = &xil_gpio_ops,
		.extra = &ps_gpio_init
	};
	struct gpio_desc *temp_desc;
	int32_t ret;

	ret = gpio_get(&temp_desc, &temp_init);
	if (ret != SUCCESS)
		return FAILURE;
	ret = gpio_direction_output(temp_desc, gpio_val);
	if (ret != SUCCESS)
		return FAILURE;
	return SUCCESS;
}





/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
 	int32_t status,ret;
 	uint16_t adc_buffer[1023 * 2] __attribute__ ((
 				aligned));

	/* Initialize SPI structures */
	struct xil_spi_init_param xil_spi_initial = {
	    .flags = 0,
		.type = SPI_PS
	};

	struct spi_init_param ad9265_spi_param = {
			.device_id = SPI_DEVICE_ID,
			.max_speed_hz =  1000000u,
			.chip_select = 0,
			.mode = SPI_MODE_0,
			.platform_ops = &xil_spi_ops,
			.extra = &xil_spi_initial
	};

	/* ADC Core */
	struct axi_adc_init ad9265_core_param = {
		.name = "ad9265_core",
		.num_channels = 2,
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *ad9265_core;

	/* AXI DMAC */
	struct axi_dmac_init ad9265_dmac_param = {
		.name = "ad9265_dmac",
		.base = RX_DMA_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0
	};
	struct axi_dmac *ad9265_dmac;

	 mdelay(500);
	 ret = adaq8092_if_gpio_setup(GPIO_1V8_ENABLE,GPIO_HIGH);

			if (ret != 0)
					return ret;


   mdelay(500);
   ret = adaq8092_if_gpio_setup(GPIO_PD1_ENABLE,GPIO_HIGH);

   			if (ret != 0)
   					return ret;

   mdelay(500);

   ret = adaq8092_if_gpio_setup(GPIO_PD2_ENABLE,GPIO_HIGH);

   		   	if (ret != 0)
   		   		     return ret;

	struct ad9265_init_param ad9265_param = {
		.spi_init = ad9265_spi_param
	};
	struct ad9265_dev *ad9265_device;

	status = axi_adc_init(&ad9265_core,  &ad9265_core_param);
	if (status != SUCCESS) {
		pr_err("axi_adc_init() error: %s\n", ad9265_core->name);
		return FAILURE;
	}

	status = axi_dmac_init(&ad9265_dmac, &ad9265_dmac_param);
	if (status != SUCCESS) {
		pr_err("axi_dmac_init() error: %s\n", ad9265_dmac->name);
		return FAILURE;
	}

	status = ad9265_setup(&ad9265_device, ad9265_param, *ad9265_core);
			if (status != SUCCESS) {
				pr_err("ad9265_setup() failed!\n");
				return FAILURE;
			}






					printf("Test pattern All Digital Outputs = 0 \n ");

				      status = ad9265_testmode_set(ad9265_device, 0x01);

					axi_dmac_transfer(ad9265_dmac, (uintptr_t)adc_buffer, sizeof(adc_buffer));

						for (int i = 0; i < 1023*2; i++) {
								if ((i % 2) == 0)
									printf("\n\r");

								printf("%d  ",adc_buffer[i]);
						}


				pr_info("\n Capture done.\n");






	pr_info("Done\n");

	return 0;
}
