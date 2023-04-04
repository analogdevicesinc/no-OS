#include <string.h>
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_rtc.h"
#include "parameters.h"
#include "hmc630x.h"
#include "iio_hmc630x.h"
#include "mwc.h"
#include "iio_app.h"
#include "led.h"
#include "net.h"

volatile bool heartbeat_pulse;

static int mwc_step(void *arg)
{
	uint8_t lock;
	struct mwc_iio_dev *mwc = arg;
	if (!heartbeat_pulse)
		return 0;
#if (TARGET_NUM == 32650)
	lock = 0;
	hmc630x_read(mwc->tx_iiodev->dev, HMC630X_LOCKDET, &lock);
	led_tx_lock(lock);
	
	lock = 0;
	hmc630x_read(mwc->rx_iiodev->dev, HMC630X_LOCKDET, &lock);
	led_rx_lock(lock);
#endif

	mwc_algorithms(mwc);

	heartbeat_pulse = false;
	return 0;
}

void heartbeat(void *context)
{
	heartbeat_pulse = true;
	no_os_rtc_set_cnt(context, 0);
}

static int heartbeat_prepare(void)
{
	int ret;
	struct no_os_rtc_init_param rtcip = {
		.id = 0,
	};

	struct no_os_rtc_desc *rtc;
	ret = no_os_rtc_init(&rtc, &rtcip);
	if (ret)
		return ret;

	ret = no_os_rtc_set_irq_time(rtc, 1);
	if (ret)
		return ret;

	struct no_os_irq_ctrl_desc *nvic;
	struct no_os_irq_init_param nvic_param = {
		.irq_ctrl_id = 0,
		.platform_ops = &max_irq_ops,
	};
	ret = no_os_irq_ctrl_init(&nvic, &nvic_param);


	struct no_os_callback_desc rtc_cb = {
		.callback = heartbeat,
		.event = NO_OS_EVT_RTC,
		.peripheral = NO_OS_RTC_IRQ,
		.ctx = rtc,
	};
	ret = no_os_irq_register_callback(nvic, RTC_IRQn, &rtc_cb);
	if (ret)
		return ret;
	ret = no_os_irq_set_priority(nvic, RTC_IRQn, 1);
	if (ret)
		return ret;
	ret = no_os_irq_enable(nvic, RTC_IRQn);
	if (ret)
		return ret;
	ret = no_os_rtc_start(rtc);
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
	uint8_t admv9615;
	struct no_os_gpio_desc *brd_select;

#if (TARGET_NUM == 32650)
	// Greeting
	ret = no_os_uart_init(&console, &uart_console_ip);
	if (ret)
		return ret;
	no_os_uart_stdio(console);
	printf("\nwethlink-firmware %s\n", NO_OS_TOSTRING(NO_OS_VERSION));

	// Detect board type switch state
	ret = no_os_gpio_get(&brd_select, &brd_select_gpio_ip);
	if (ret)
		goto end;
	ret = no_os_gpio_direction_input(brd_select);
	if (ret)
		goto end;
	ret = no_os_gpio_get_value(brd_select, &admv9615);
	if (ret)
		goto end;
#else
	admv9615 = false;
#endif
	if (admv9615)
		strcpy(hw_model_str, "admv9615");
	else
		strcpy(hw_model_str, "admv9625");

	printf("Board: %s\n", hw_model_str);
#if (TARGET_NUM == 32650)
	ret = led_init();
	if (ret)
		goto end;
#endif

	const uint64_t txfreq = admv9615 ? 63000000000 : 58012500000;
	const uint64_t rxfreq = admv9615 ? 58012500000 : 63000000000;

	struct mwc_iio_dev *mwc;
	struct mwc_iio_init_param mwc_ip = {
		.reset_gpio_ip = &xcvr_reset_gpio_ip,
		.tx_autotuning = true,
		.tx_target = 350,
		.tx_tolerance = 50,
		.rx_autotuning = true,
		.rx_target = 1950,
		.rx_tolerance = 50,
		.tx_auto_ifvga = true,
		.rx_auto_ifvga_rflna = true,
		.hbtx = admv9615,
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
	txip.vco = txfreq;
	txip.enabled = true;
	txip.temp_en = true;
	txip.if_attn = 13;
	txip.tx.rf_attn = 15;
	struct hmc630x_iio_init_param iio_txip = {
		.ip = &txip,
	};
	ret = hmc630x_iio_init(&iio_tx, &iio_txip);
	if (ret)
		goto end;

	// admv9615 specific initializations
	ret = hmc630x_write(iio_tx->dev, HMC6300_PA_SEL_VREF, 0x8);
	if (ret)
		goto end;
	ret = hmc630x_write(iio_tx->dev, HMC6300_PA_PWRDWN_FAST, 0);
	if (ret)
		goto end;
	ret = hmc630x_write(iio_tx->dev, HMC6300_PA_SE_SEL, 0);
	if (ret)
		goto end;

	struct hmc630x_iio_dev *iio_rx;
	struct hmc630x_init_param rxip = {0};
	rxip.type = HMC6301;
	rxip.ref_clk = HMC6300_REF_CLK_75MHz;
	rxip.en = xcvr_en_gpio_ip;
	rxip.clk = xcvr_clk_gpio_ip;
	rxip.data = xcvr_data_gpio_ip;
	rxip.scanout = xcvr_scanout_rx_gpio_ip;
	rxip.vco = rxfreq;
	rxip.enabled = true;
	rxip.temp_en = true;
	rxip.if_attn = 11;
	rxip.rx.bb_attn1 = HMC6301_BB_ATTN_18dB;
	rxip.rx.bb_attn2 = HMC6301_BB_ATTN_18dB;
	rxip.rx.bb_attni_fine = HMC6301_BB_ATTN_FINE_3dB;
	rxip.rx.bb_attnq_fine = HMC6301_BB_ATTN_FINE_0dB;
	rxip.rx.lna_gain = HMC6301_LNA_GAIN_12dB;
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
	};

	struct iio_ctx_attr iio_ctx_attrs[] = {
		{
			.name = "hw_model",
			.value = hw_model_str,
		},
		{
			.name = "hw_version",
			.value = "TODO-READ-VERSION-FROM-EEPROM"
		},
		{
			.name = "hw_serial",
			.value = "TODO-READ-SERIAL-NO-FROM-EEPROM"
		},
		{
			.name = "carrier_model",
			.value = "ETHERNET-MICROWAVE-EVAL"
		},
		{
			.name = "carrier_version",
			.value = "TODO-READ-VERSION-FROM-EEPROM"
		},
		{
			.name = "carrier_serial",
			.value = "TODO-READ-SERIAL-NO-FROM-EEPROM"
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

	ret = heartbeat_prepare();
	if (ret)
		goto end;
#if (TARGET_NUM == 32650)
	mwc_algorithms(mwc);

	ret = net_init(admv9615);
	if (ret)
		goto end;
#endif

	iio_app_run(app);
end:
	printf("End of program: %d\n", ret);
	iio_app_remove(app);
	hmc630x_iio_remove(iio_tx);
	hmc630x_iio_remove(iio_rx);
	
	return 0;
}
