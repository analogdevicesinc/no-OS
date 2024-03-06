#include <stdio.h>
#include "no_os_delay.h"
#include "no_os_mdio.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "mdio_bitbang.h"
#include "net.h"
#include "led.h"
#include "parameters.h"
#include "adin1300.h"
#include "iio_adin1300.h"
#include "max24287.h"
#include "iio_max24287.h"

struct net_context {
	struct adin1300_desc *adin1300;
	struct max24287_desc *max24287;
	struct no_os_irq_ctrl_desc *nvic;
};

volatile bool selfchange;

void adin1300_int(void *context)
{
	uint16_t spd;
	uint16_t status = 0;
	struct net_context *ctx = context;

	no_os_irq_disable(ctx->nvic, ADIN1300_INT_IRQn);

	// this also clears the flags
	adin1300_read(ctx->adin1300, ADIN1300_IRQ_STATUS, &status);

	if (status & ADIN1300_LNK_STAT_CHNG_IRQ_MASK) {
		spd = adin1300_resolved_speed(ctx->adin1300);

		if (spd <= 5) {
			led_rj45((enum rj45_led)spd / 2);
			max24287_config_parallel(ctx->max24287, MAX24287_RGMII, spd);
			max24287_config_serial(ctx->max24287, MAX24287_SGMII, spd);


			if (selfchange)
				selfchange = false;
			else
				max24287_write_bits(ctx->max24287, MAX24287_BMCR,
						    MAX24287_AN_EN_MASK | MAX24287_AN_START_MASK,
						    MAX24287_AN_EN_MASK | MAX24287_AN_START_MASK);
		} else {
			// cable disconnected, advertise all speeds
			led_rj45(rj45_led_off);
			max24287_config_serial(ctx->max24287, MAX24287_SGMII, 5);
			adin1300_config_speed(ctx->adin1300, 5);
		}
	}

	no_os_irq_enable(ctx->nvic, ADIN1300_INT_IRQn);
}

void max24287_int(void *context)
{
	struct net_context *ctx = context;
	uint16_t reg;
	enum max24287_speed remote_speed;
	enum adin1300_speed local_speed;

	no_os_irq_disable(ctx->nvic, MAX24287_INT_IRQn);
	max24287_read(ctx->max24287, MAX24287_IR, NULL);
	max24287_read(ctx->max24287, MAX24287_AN_EXP, &reg);

	if (reg & MAX24287_AN_RX_PAGE) {
		max24287_read(ctx->max24287, MAX24287_AN_RX, &reg);
		remote_speed = (no_os_field_get(MAX24287_SPD_MASK, reg) << 1) |
			       no_os_field_get(MAX24287_DPLX_MASK, reg);
		max24287_read(ctx->max24287, MAX24287_AN_ADV, &reg);
		local_speed = adin1300_resolved_speed(ctx->adin1300);

		if (remote_speed < local_speed) {
			max24287_config_parallel(ctx->max24287, MAX24287_RGMII, remote_speed);
			max24287_config_serial(ctx->max24287, MAX24287_SGMII, remote_speed);
			adin1300_config_speed(ctx->adin1300, remote_speed);
			selfchange = true;
			adin1300_autoneg(ctx->adin1300, true);
		}
	}

	no_os_irq_enable(ctx->nvic, MAX24287_INT_IRQn);
}

int net_init(struct adin1300_iio_desc **adin1300_iio,
	     struct max24287_iio_desc **max24287_iio,
	     unsigned int speed)
{
	int ret;
	static struct net_context ctx;
	enum adin1300_speed phy_speed_cap;
	struct adin1300_desc *adin1300;
	struct max24287_desc *max24287;
	struct no_os_gpio_desc *int_gpio;
	struct no_os_gpio_desc *int2_gpio;
	struct no_os_irq_ctrl_desc *nvic;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};
	struct no_os_irq_ctrl_desc *gpio2intc;
	struct no_os_irq_init_param gpio2intc_ip = {
		.irq_ctrl_id = ADIN1300_INT_PORT,
		.platform_ops = &max_gpio_irq_ops,
	};
	struct no_os_callback_desc adin1300_int_cb = {
		.callback = adin1300_int,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};
	struct no_os_irq_ctrl_desc *gpio1intc;
	struct no_os_irq_init_param gpio1intc_ip = {
		.irq_ctrl_id = MAX24287_INT_PORT,
		.platform_ops = &max_gpio_irq_ops,
	};
	struct no_os_callback_desc max24287_int_cb = {
		.callback = max24287_int,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};

	switch(speed) {
	case 10:
		phy_speed_cap = ADIN1300_10_BASE_T_FULLDUPLEX;
		break;
	case 100:
		phy_speed_cap = ADIN1300_100_BASE_TX_FULLDUPLEX;
		break;
	case 1000:
	default:
		phy_speed_cap = ADIN1300_1000_BASE_T_FULLDUPLEX;
		break;
	};

	struct adin1300_init_param adin1300_ip = {
		.reset_param = &adin1300_reset_gpio_ip,
		.mdio_param = {
			.c45 = true,
			.addr = 0,
			.ops = &mdio_bitbang_ops,
			.extra = &(struct mdio_bitbang_init_param)
			{
				.mdc = adin1300_mdc_gpio_ip,
				.mdio = adin1300_mdio_gpio_ip,
			},
		},
		.rgmii_config = {
			.tx_idelay_en = true,
			.rx_idelay_en = true
		},
		.gp_clk = true,
		.gp_clk_source = ADIN1300_GP_CLK_RECOVERED,
		.clk25_ref = true,
		.speed_cap = phy_speed_cap,
		.autoneg = true,
	};
	ret = adin1300_init(&adin1300, &adin1300_ip);
	if (ret)
		return ret;

	// See datasheet section "Back to Back Mode in 10BASE-T" to understand this.
	ret = adin1300_write(adin1300, ADIN1300_GE_B10_REGEN_PRE,
			     ADIN1300_GE_B10_REGEN_PRE_MSK);
	if (ret)
		return ret;

	ret = adin1300_iio_init(adin1300_iio,
	&(struct adin1300_iio_init_param) {
		.dev = adin1300
	});
	if (ret)
		return ret;

	// SERDES
	struct max24287_init_param serdes_ip = {
		.reset_param = &max24287_reset_gpio_ip,
		.mdio_param = {
			.addr = 0x7,
			.ops = &mdio_bitbang_ops,
			.extra = &(struct mdio_bitbang_init_param)
			{
				.mdc = max24287_mdc_gpio_ip,
				.mdio = max24287_mdio_gpio_ip,
			},
		},
		.parallel = MAX24287_RGMII,
		.parspeed = MAX24287_1000_FULLDUPLEX, // we go with full speed for now, will be adjusted by interrupt
		.serial = MAX24287_SGMII,
		.serspeed = MAX24287_1000_FULLDUPLEX,
	};
	ret = max24287_init(&max24287, &serdes_ip);
	if (ret)
		return ret;

	// select GPIO1, GPIO2 & GPIO3 functions
	// 0xE5: GPIO1 interrupt, GPIO2 125 MHz TX PLL, GPIO3 25MHz or 125MHz from receive clock recovery PLL
	max24287_write(max24287, MAX24287_GPIOCR1, 0xE5);

	ret = max24287_iio_init(max24287_iio,
	&(struct max24287_iio_init_param) {
		.dev = max24287
	});
	if (ret)
		return ret;

	// configure the phy interrupt pin
	ret = no_os_gpio_get(&int_gpio, &adin1300_int_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(int_gpio);
	if (ret)
		return ret;

	adin1300_read(adin1300, ADIN1300_IRQ_STATUS, NULL);
	adin1300_write(adin1300, ADIN1300_IRQ_MASK,
		       ADIN1300_HW_IRQ_EN_MASK | ADIN1300_LNK_STAT_CHNG_IRQ_MASK);

	ret = no_os_irq_ctrl_init(&nvic, &nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_enable(nvic, ADIN1300_INT_IRQn);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&gpio2intc, &gpio2intc_ip);
	if (ret)
		return ret;

	ctx.adin1300 = adin1300;
	ctx.max24287 = max24287;
	ctx.nvic = nvic;
	adin1300_int_cb.ctx = &ctx;
	ret = no_os_irq_register_callback(gpio2intc, ADIN1300_INT_PIN,
					  &adin1300_int_cb);
	if (ret)
		return ret;

	ret = no_os_irq_trigger_level_set(gpio2intc, ADIN1300_INT_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(gpio2intc, ADIN1300_INT_PIN, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(gpio2intc, ADIN1300_INT_PIN);
	if (ret)
		return ret;

	// configure ser/des interrupt
	ret = no_os_gpio_get(&int2_gpio, &max24287_int_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(int2_gpio);
	if (ret)
		return ret;

	max24287_read(max24287, MAX24287_AN_EXP, NULL);
	max24287_write(max24287, MAX24287_IR, MAX24287_PAGE_IE);

	ret = no_os_irq_enable(nvic, MAX24287_INT_IRQn);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&gpio1intc, &gpio1intc_ip);
	if (ret)
		return ret;

	max24287_int_cb.ctx = &ctx;
	ret = no_os_irq_register_callback(gpio1intc, MAX24287_INT_PIN,
					  &max24287_int_cb);
	if (ret)
		return ret;

	ret = no_os_irq_trigger_level_set(gpio1intc, MAX24287_INT_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(gpio1intc, MAX24287_INT_PIN, 1);
	if (ret)
		return ret;

	return no_os_irq_enable(gpio1intc, MAX24287_INT_PIN);
}
