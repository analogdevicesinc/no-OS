#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "hmc6300.h"

// select between admv9611 (high-band tx) and admv9621 (low-band tx)
const bool admv9611 = false;

int main()
{
	int ret;
	struct no_os_uart_desc *uart_desc;
	struct no_os_gpio_desc *reset;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	maxim_uart_stdio(uart_desc);

	struct hmc6300_dev *tx;
	struct hmc6300_init_param txip = {0};
	reset_gpio_ip.extra = &xgpio;
	en_gpio_ip.extra = &xgpio;
	clk_gpio_ip.extra = &xgpio;
	data_gpio_ip.extra = &xgpio;
	scanout_tx_gpio_ip.extra = &xgpio;
	txip.ref_clk = HMC6300_REF_CLK_75MHz;
	txip.en = en_gpio_ip;
	txip.clk = clk_gpio_ip;
	txip.data = data_gpio_ip;
	txip.scanout = scanout_tx_gpio_ip;

	// initialize reset gpio separately
	ret = no_os_gpio_get(&reset, &reset_gpio_ip);
	if (ret)
		goto end;

	// set gpio direction
	ret = no_os_gpio_direction_output(reset, NO_OS_GPIO_LOW);
	if (ret)
		goto end;

	// reset
	no_os_gpio_set_value(reset, NO_OS_GPIO_HIGH);
	no_os_mdelay(1);
	no_os_gpio_set_value(reset, NO_OS_GPIO_LOW);
	no_os_mdelay(1);

	ret = hmc6300_init(&tx, &txip);
	if (ret) {
		printf("Failed to initialize.\n");
		goto end;
	}

	// clear PA_PWRDWN_FAST (is this needed ? default is 1)
	ret = hmc6300_write(tx, HMC6300_PA_PWRDWN_FAST, 0);
	if (ret)
		goto end;

	// Enable temperature sensor
	ret = hmc6300_enable_temp(tx, true);
	if (ret)
		goto end;

	// set IF attenuation
	ret = hmc6300_set_if_attn(tx, 12);
	if (ret)
		goto end;

	// set RF attenuation
	ret = hmc6300_set_rf_attn(tx, 15);
	if (ret)
		goto end;

	// set VCO frequency
	ret = hmc6300_set_vco(tx, 58012500000);
	if (ret)
		goto end;

	// print the whole register map
	int r;
	uint8_t regmap[32];
	ret = hmc6300_read_regmap(tx, regmap);
	if (ret)
		goto end;
	for (r = 0; r < 28; r++) {
		printf("Row %d: 0x%x (%d)\n", r, regmap[r], regmap[r]);
	}

	// read the temperature
	uint8_t temp;
	ret = hmc6300_get_temp(tx, &temp);
	if (ret)
		goto end;
	printf("Temperature: 0x%x\n", temp);

end:
	printf("%s returned with %d\n", __func__, ret);
	return ret;
}
