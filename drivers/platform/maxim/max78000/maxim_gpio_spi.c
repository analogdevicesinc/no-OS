/*******************************************************************************
 *   @file   maxim_gpio_spi.c
 *   @brief  Bit-banged (GPIO) SPI master for MAX78000, no_os_spi platform ops.
 *   @author Victor Pascu (victor.pascu@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#include <errno.h>
#include "maxim_gpio_spi.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "mxc_device.h"
#include "gpio_regs.h"
#include "gcr_regs.h"

/**
 * @struct max_gpio_spi_state
 * @brief  Resolved, per-descriptor register view cached at init so the bit loop
 *         does no lookups. Stored in no_os_spi_desc.extra.
 */
struct max_gpio_spi_state {
	mxc_gpio_regs_t *gpio;   /* resolved GPIO register block */
	uint32_t sclk;           /* NO_OS_BIT(sclk_pin) */
	uint32_t mosi;           /* NO_OS_BIT(mosi_pin) */
	uint32_t miso;           /* NO_OS_BIT(miso_pin) */
	uint32_t cs;             /* NO_OS_BIT(cs_pin) */
	uint32_t bus;            /* sclk | mosi (shared, driven only while active) */
	uint32_t half_period_nops;
};

static inline void max_gpio_spi_bit_delay(const struct max_gpio_spi_state *st)
{
	for (volatile uint32_t i = 0; i < st->half_period_nops; i++)
		__asm__ volatile("nop");
}

/* Configure MISO as a high-Z input at the target rail, no pull. */
static void max_gpio_spi_release_miso(struct max_gpio_spi_state *st)
{
	mxc_gpio_regs_t *g = st->gpio;

	g->inen |= st->miso;
	g->outen_clr = st->miso;
	g->en0_set = st->miso;
	g->en1_clr = st->miso;
	g->en2_clr = st->miso;
	g->padctrl0 &= ~st->miso;
	g->padctrl1 &= ~st->miso;
	g->ps &= ~st->miso;
}

/*
 * Claim the shared SCLK+MOSI lines: drive them push-pull with SCLK idle high
 * (mode 3, CPOL=1) and MOSI low. Call while the caller holds any cross-core bus
 * lock, so no other core is driving the bus.
 */
static void max_gpio_spi_bus_acquire(struct max_gpio_spi_state *st)
{
	mxc_gpio_regs_t *g = st->gpio;

	g->out_set = st->sclk;
	g->out_clr = st->mosi;
	g->outen_set = st->bus;
}

/* Release SCLK+MOSI back to high-Z inputs so another core can own the bus. */
static void max_gpio_spi_bus_release(struct max_gpio_spi_state *st)
{
	mxc_gpio_regs_t *g = st->gpio;

	g->outen_clr = st->bus;
	g->inen |= st->bus;
}

static inline void max_gpio_spi_cs(struct max_gpio_spi_state *st, bool high)
{
	if (high)
		st->gpio->out_set = st->cs;
	else
		st->gpio->out_clr = st->cs;
}

/* One SPI mode-3 byte: drive MOSI, falling edge, rising edge, sample MISO. */
static uint8_t max_gpio_spi_xfer_byte(struct max_gpio_spi_state *st, uint8_t tx)
{
	mxc_gpio_regs_t *g = st->gpio;
	uint8_t rx = 0;

	for (uint8_t bit = 0; bit < 8; bit++) {
		if (tx & 0x80)
			g->out_set = st->mosi;
		else
			g->out_clr = st->mosi;
		max_gpio_spi_bit_delay(st);
		g->out_clr = st->sclk;
		max_gpio_spi_bit_delay(st);
		g->out_set = st->sclk;
		max_gpio_spi_bit_delay(st);
		rx <<= 1;
		if (g->in & st->miso)
			rx |= 1;
		tx <<= 1;
	}

	return rx;
}

static int32_t max_gpio_spi_init(struct no_os_spi_desc **desc,
				 const struct no_os_spi_init_param *param)
{
	const struct max_gpio_spi_init_param *eparam;
	struct max_gpio_spi_state *st;
	struct no_os_spi_desc *descriptor;
	mxc_gpio_regs_t *g;
	uint32_t all;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	/* This bit-bang backend implements SPI mode 3, MSB-first only. */
	if (param->mode != NO_OS_SPI_MODE_3 ||
	    param->bit_order != NO_OS_SPI_BIT_ORDER_MSB_FIRST)
		return -ENOTSUP;

	eparam = param->extra;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	st = no_os_calloc(1, sizeof(*st));
	if (!st) {
		no_os_free(descriptor);
		return -ENOMEM;
	}

	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;
	descriptor->bit_order = param->bit_order;
	descriptor->extra = st;

	st->gpio = MXC_GPIO_GET_GPIO(eparam->port);
	st->sclk = NO_OS_BIT(eparam->sclk_pin);
	st->mosi = NO_OS_BIT(eparam->mosi_pin);
	st->miso = NO_OS_BIT(eparam->miso_pin);
	st->cs   = NO_OS_BIT(eparam->cs_pin);
	st->bus  = st->sclk | st->mosi;
	st->half_period_nops = eparam->half_period_nops;
	g = st->gpio;
	all = st->sclk | st->mosi | st->miso | st->cs;

	/* Ungate the GPIO block (harmless if already enabled). */
	MXC_GCR->pclkdis0 &= ~MXC_F_GCR_PCLKDIS0_GPIO0;

	/* Select the requested I/O rail for every pin we touch. */
	if (eparam->vddioh)
		g->vssel |= all;
	else
		g->vssel &= ~all;

	/* Put all pins in GPIO I/O mode (en0=1, en1=en2=0). */
	g->en0_set = all;
	g->en1_clr = all;
	g->en2_clr = all;

	/*
	 * Single-driver discipline: CS is the only persistent output owned by this
	 * descriptor; drive it deasserted (high). SCLK+MOSI stay high-Z until a
	 * transfer takes the bus; MISO is always an input.
	 */
	g->outen_set = st->cs;
	max_gpio_spi_cs(st, true);
	max_gpio_spi_release_miso(st);
	max_gpio_spi_bus_release(st);

	*desc = descriptor;
	return 0;
}

static int32_t max_gpio_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc->extra);
	no_os_free(desc);
	return 0;
}

static int32_t max_gpio_spi_transfer(struct no_os_spi_desc *desc,
				     struct no_os_spi_msg *msgs, uint32_t len)
{
	struct max_gpio_spi_state *st;

	if (!desc || !desc->extra || !msgs || !len)
		return -EINVAL;

	st = desc->extra;

	/*
	 * The caller is expected to hold any cross-core bus lock across this whole
	 * call, so we take ownership of the shared SCLK+MOSI lines now and hand them
	 * back at the end. MISO stays an input; CS is already owned by this core.
	 */
	max_gpio_spi_release_miso(st);
	max_gpio_spi_bus_acquire(st);

	for (uint32_t i = 0; i < len; i++) {
		struct no_os_spi_msg *msg = &msgs[i];

		max_gpio_spi_cs(st, true);
		st->gpio->out_set = st->sclk;   /* SCLK idle high */
		max_gpio_spi_bit_delay(st);
		no_os_udelay(msg->cs_delay_first);
		max_gpio_spi_cs(st, false);
		max_gpio_spi_bit_delay(st);

		for (uint32_t b = 0; b < msg->bytes_number; b++) {
			uint8_t tx = msg->tx_buff ? msg->tx_buff[b] : 0;
			uint8_t rx = max_gpio_spi_xfer_byte(st, tx);

			if (msg->rx_buff)
				msg->rx_buff[b] = rx;
		}

		no_os_udelay(msg->cs_delay_last);
		if (msg->cs_change) {
			max_gpio_spi_cs(st, true);
			max_gpio_spi_bit_delay(st);
			no_os_udelay(msg->cs_change_delay);
		}
	}

	max_gpio_spi_cs(st, true);
	max_gpio_spi_bus_release(st);
	return 0;
}

static int32_t max_gpio_spi_write_and_read(struct no_os_spi_desc *desc,
		uint8_t *data, uint16_t bytes_number)
{
	struct no_os_spi_msg msg = {
		.tx_buff = data,
		.rx_buff = data,
		.bytes_number = bytes_number,
		.cs_change = 1,
	};

	return max_gpio_spi_transfer(desc, &msg, 1);
}

/**
 * @brief Bit-banged SPI platform ops. See maxim_gpio_spi.h for the pin map and
 * the single-driver bus-ownership contract.
 */
const struct no_os_spi_platform_ops max_gpio_spi_ops = {
	.init = max_gpio_spi_init,
	.write_and_read = max_gpio_spi_write_and_read,
	.transfer = max_gpio_spi_transfer,
	.remove = max_gpio_spi_remove,
};
