#include <string.h>
#include "no_os_init.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_timer.h"
#include "maxim_timer.h"
#include "no_os_crc8.h"
#include "no_os_mdio.h"
#include "mdio_bitbang.h"
#include "parameters.h"
#include "hmc630x.h"
#include "iio_hmc630x.h"
#include "mwc.h"
#include "net.h"
#include "iio_app.h"
#include "dp83tg.h"
#include "iio_dp83tg.h"
#include "mxc_sys.h"

volatile bool heartbeat_pulse = false;

static int mwc_step(void *arg)
{
	uint8_t lock;
	uint16_t reg;
	static bool temp = false;
	struct mwc_iio_dev *mwc = arg;
	if (!heartbeat_pulse)
		return 0;

	lock = 0;
	hmc630x_read(mwc->tx_iiodev->dev, HMC630X_LOCKDET, &lock);

	lock = 0;
	hmc630x_read(mwc->rx_iiodev->dev, HMC630X_LOCKDET, &lock);

	mwc_algorithms(mwc);

	if (!link)
	{
		dp83tg_read(mwc->dp83tg, DP83TG_INTERRUPT_STATUS_1, &reg);
		reg >>= 8;
		if ((reg & DP83TG_ENERGY_DETECT_MASK) && !(reg & DP83TG_TRAINING_DONE_MASK) && !temp)
		{
			temp = true;
			dp83tg_read(mwc->dp83tg, DP83TG_PMA_PMD_CONTROL, &reg);
			bool master = reg & DP83TG_CFG_MASTER_SLAVE_MASK;

			dp83tg_config(mwc->dp83tg, !master);
		}
	}
	else
		temp = false;

	heartbeat_pulse = false;
	return 0;
}

void heartbeat(void *context)
{
	heartbeat_pulse = true;
	uint32_t count;
	no_os_timer_counter_get(context, &count);
}

static int heartbeat_prepare(void)
{
	int ret;

	struct no_os_timer_init_param tim_config = {
		.id = 1,
		.freq_hz = PeripheralClock / 1024,
		.ticks_count = PeripheralClock / 1024 / 2,
		.platform_ops = &max_timer_ops,
	};

	struct no_os_timer_desc *tim;
	ret = no_os_timer_init(&tim, &tim_config);
	if (ret)
		return ret;
 
	/* Timer irq initialization parameter */
	struct no_os_irq_init_param irq_config = {
		.irq_ctrl_id = 0, // specific to NVIC IRQ controller
		.platform_ops = &max_irq_ops, // platform specific
	};
 
	struct no_os_callback_desc irq_cb = {
		.callback = heartbeat,
		.ctx = tim,
		.event = NO_OS_EVT_TIM_ELAPSED,
		.peripheral = NO_OS_TIM_IRQ,
		.handle = MXC_TMR1,
	};

	/* Irq descriptor */
	struct no_os_irq_ctrl_desc *nvic;

	/* Initialize timer IRQ controller */
	ret = no_os_irq_ctrl_init(&nvic, &irq_config);
	if (ret)
		return ret;
 
	/* Set timer IRQ priority */
	ret = no_os_irq_set_priority(nvic, TMR1_IRQn, 1);
	if (ret)
		return ret;
 
	/* Register callback for timer IRQ */
	ret = no_os_irq_register_callback(nvic, TMR1_IRQn, &irq_cb);
	if (ret)
		return ret;
 
	/* Enable timer IRQ */
	ret = no_os_irq_enable(nvic, TMR1_IRQn);
	if (ret)
		return ret;

	/* Start timer */	 
	ret = no_os_timer_start(tim);
	if (ret)	 
		return ret;	

	return 0;
}

#define NO_OS_STRINGIFY(x) #x
#define NO_OS_TOSTRING(x) NO_OS_STRINGIFY(x)

int main(void)
{
	int ret;
	struct no_os_uart_desc *console;
	char hw_model_str[10];
	enum admv96xx_id id = ID_ADMV96X7;
	uint8_t hbtx;
	uint8_t pin;
	uint8_t crc;
	struct no_os_gpio_desc *brd_select;
	struct no_os_gpio_desc *factory_defaults_gpio;
	struct no_os_eeprom_desc *eeprom;
	const uint16_t nvmpsz = sizeof(union nvmp255);
	uint8_t eebuf[nvmpsz + 1];
	union nvmp255 *nvmp;
	struct dp83tg_iio_desc *iio_dp83tg;

	NO_OS_DECLARE_CRC8_TABLE(crc8);
	no_os_crc8_populate_msb(crc8, 0x7);

	no_os_init();

	// Greeting
	struct no_os_uart_init_param uart_greeting_ip = uart_console_ip;
	uart_greeting_ip.baud_rate = 115200;
	ret = no_os_uart_init(&console, &uart_greeting_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(console);
	printf("\nno-OS: %s\n", NO_OS_TOSTRING(NO_OS_VERSION));

	// Detect board type switch state
	ret = no_os_gpio_get(&brd_select, &brd_select_gpio_ip);
	if (ret)
		goto end;
	ret = no_os_gpio_direction_input(brd_select);
	if (ret)
		goto end;
	ret = no_os_gpio_get_value(brd_select, &hbtx);
	if (ret)
		goto end;

	sprintf(hw_model_str, "admv96%d%d", hbtx ? 1 : 2, id);

	printf("Transceiver: %s\n", hw_model_str);

	ret = no_os_eeprom_init(&eeprom, &eeprom_ip);
	if (ret)
		goto end;

	// Detect request to reset to factory defaults
	ret = no_os_gpio_get(&factory_defaults_gpio, &factory_defaults_gpio_ip);
	if (ret)
		goto end;
	ret = no_os_gpio_direction_input(factory_defaults_gpio);
	if (ret)
		goto end;
	ret = no_os_gpio_get_value(factory_defaults_gpio, &pin);
	if (ret)
		goto end;

	if (!pin)
apply_factory_defaults: {
		printf("EEPROM: loading factory defaults...\n");
		ret = no_os_eeprom_read(eeprom, NVMP_AREA_ADDRESS(15), eebuf, nvmpsz+1);
		if (ret)
			return ret;

		crc = no_os_crc8(crc8, eebuf, nvmpsz, 0xa5);
		if (crc == eebuf[nvmpsz]) {
			ret = no_os_eeprom_write(eeprom, NVMP_AREA_ADDRESS(0), eebuf, nvmpsz+1);
			if (ret)
				return ret;
			printf("EEPROM: loaded factory defaults.\n");

		} else {
			printf("EEPROM: CRC mismatch, read 0x%x, computed 0x%x\n", eebuf[nvmpsz], crc);
			printf("EEPROM: cannot load bad factory defaults.\n");

			memcpy(eebuf, &factory_defaults_template, nvmpsz);
			printf("EEPROM: loaded hardcoded parameters instead.\n");

			goto post_eeprom;
		}
	}

	printf("EEPROM: loading non-volatile parameters...\n");
	ret = no_os_eeprom_read(eeprom, NVMP_AREA_ADDRESS(0), eebuf, nvmpsz+1);
	if (ret)
		return ret;

	crc = no_os_crc8(crc8, eebuf, nvmpsz, 0xa5);
	if (crc != eebuf[nvmpsz]) {
		printf("EEPROM: CRC mismatch, read 0x%x, computed 0x%x\n", eebuf[nvmpsz], crc);
		printf("EEPROM: cannot load bad non-volatile parameters.\n");
		goto apply_factory_defaults;
	}
	printf("EEPROM: loaded non-volatile parameters.\n");
post_eeprom:
	nvmp = (union nvmp255 *)eebuf;

	ret = net_init(&iio_dp83tg, !hbtx);
	if (ret)
		goto end;

	struct mwc_iio_dev *mwc;
	struct mwc_iio_init_param mwc_ip = {
		.reset_gpio_ip = &xcvr_reset_gpio_ip,
		.tx_autotuning = nvmp->data.tx_autotuning,
		.tx_target = nvmp->data.tx_target,
		.tx_tolerance = nvmp->data.tx_tolerance,
		.rx_autotuning = nvmp->data.rx_autotuning,
		.rx_target = nvmp->data.rx_target,
		.rx_tolerance = nvmp->data.rx_tolerance,
		.tx_auto_ifvga = nvmp->data.tx_auto_ifvga,
		.rx_auto_ifvga_rflna = nvmp->data.rx_auto_ifvga_rflna,
		.temp_correlation = &nvmp->data.temp_correlation[hbtx],
		.id = id,
		.hbtx = hbtx,
		.crc8 = crc8,
		.eeprom = eeprom,
		.dp83tg = iio_dp83tg->dev,
	};
	ret = mwc_iio_init(&mwc, &mwc_ip);
	if (ret)
		goto end;

	ret = mwc_tx_rx_reset(mwc);
	if (ret)
		goto end;

	struct hmc630x_iio_dev *iio_tx;
	struct hmc630x_init_param txip = {0};
	txip.type = HMC6300;
	txip.ref_clk = HMC6300_REF_CLK_75MHz;
	txip.en = xcvr_en_gpio_ip;
	txip.clk = xcvr_clk_gpio_ip;
	txip.data = xcvr_data_gpio_ip;
	txip.scanout = xcvr_scanout_tx_gpio_ip;
	txip.vco = nvmp->data.hmc6300_vco[hbtx];
	txip.enabled = nvmp->data.hmc6300_enabled;
	txip.temp_en = true;
	txip.if_attn = nvmp->data.hmc6300_if_attn;
	txip.tx.rf_attn = nvmp->data.hmc6300_rf_attn;
	struct hmc630x_iio_init_param iio_txip = {
		.ip = &txip,
	};
	ret = hmc630x_iio_init(&iio_tx, &iio_txip);
	if (ret)
		goto end;

	if (id == ID_ADMV96X5 || id == ID_ADMV96X7) {
		ret = hmc630x_write(iio_tx->dev, HMC6300_PA_SEL_VREF, 0x8);
		if (ret)
			goto end;
		ret = hmc630x_write(iio_tx->dev, HMC6300_PA_PWRDWN_FAST, 0);
		if (ret)
			goto end;
		ret = hmc630x_write(iio_tx->dev, HMC6300_PA_SE_SEL, 0);
		if (ret)
			goto end;
	}

	struct hmc630x_iio_dev *iio_rx;
	struct hmc630x_init_param rxip = {0};
	rxip.type = HMC6301;
	rxip.ref_clk = HMC6300_REF_CLK_75MHz;
	rxip.en = xcvr_en_gpio_ip;
	rxip.clk = xcvr_clk_gpio_ip;
	rxip.data = xcvr_data_gpio_ip;
	rxip.scanout = xcvr_scanout_rx_gpio_ip;
	rxip.vco = nvmp->data.hmc6301_vco[hbtx];
	rxip.enabled = nvmp->data.hmc6301_enabled;
	rxip.temp_en = true;
	rxip.if_attn = nvmp->data.hmc6301_if_attn;
	rxip.rx.bb_attn1 = nvmp->data.hmc6301_bb_attn1;
	rxip.rx.bb_attn2 = nvmp->data.hmc6301_bb_attn2;
	rxip.rx.bb_attni_fine = nvmp->data.hmc6301_bb_attni_fine;
	rxip.rx.bb_attnq_fine = nvmp->data.hmc6301_bb_attnq_fine;
	rxip.rx.lna_attn = nvmp->data.hmc6301_lna_attn;
	rxip.rx.bb_lpc = HMC6301_BB_LPC_1400MHz;
	rxip.rx.bb_hpc = HMC6301_BB_HPC_45kHz;
	struct hmc630x_iio_init_param iio_rxip = {
		.ip = &rxip,
	};
	ret = hmc630x_iio_init(&iio_rx, &iio_rxip);
	if (ret)
		goto end;

	// TODO: do this in a better way.
	mwc->tx_iiodev = iio_tx;
	mwc->rx_iiodev = iio_rx;

	ret = heartbeat_prepare();
	if (ret)
		goto end;

	struct iio_app_device iio_devices[] = {
		{
			.name = "hmc6300",
			.dev = iio_tx,
			.dev_descriptor = iio_tx->iio_dev,
		},
		{
			.name = "hmc6301",
			.dev = iio_rx,
			.dev_descriptor = iio_rx->iio_dev,
		},
		{
			.name = "mwc",
			.dev = mwc,
			.dev_descriptor = mwc->iio_dev,
		},
		{
			.name = "dp83tg",
			.dev = iio_dp83tg,
			.dev_descriptor = iio_dp83tg->iio_dev,
		},
	};

	struct iio_ctx_attr iio_ctx_attrs[] = {
		{
			.name = "hw_model",
			.value = hw_model_str,
		},
		{
			.name = "hw_version",
			.value = nvmp->data.hw_version
		},
		{
			.name = "hw_serial",
			.value = nvmp->data.hw_serial
		},
		{
			.name = "carrier_model",
			.value = nvmp->data.carrier_model
		},
		{
			.name = "carrier_version",
			.value = nvmp->data.carrier_version
		},
		{
			.name = "carrier_serial",
			.value = nvmp->data.carrier_serial
		},
	};

	struct iio_app_init_param aip = {
		.ctx_attrs = iio_ctx_attrs,
		.nb_ctx_attr = NO_OS_ARRAY_SIZE(iio_ctx_attrs),
		.devices = iio_devices,
		.nb_devices = NO_OS_ARRAY_SIZE(iio_devices),
		.uart_init_params = uart_console_ip,
		.post_step_callback = mwc_step,
		.arg = mwc,
	};

	struct iio_app_desc *app;
	ret = iio_app_init(&app, aip);
	if (ret)
		goto end;

	iio_app_run(app);
end:
	printf("End of program: %d\n", ret);
	iio_app_remove(app);
	hmc630x_iio_remove(iio_tx);
	hmc630x_iio_remove(iio_rx);

	return 0;
}
