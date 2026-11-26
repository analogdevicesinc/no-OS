/***************************************************************************//**
 *   @file   sim_platform.c
 *   @brief  Software-in-the-loop stand-ins for the AD9082/VPK180 hardware.
 *
 *           Built only with -DAD9082_SIM=ON.  The project sources keep using
 *           xil_spi_ops / xil_gpio_ops; CMake renames them to sim_spi_ops /
 *           sim_gpio_ops for the ad9082 target, and wraps
 *           no_os_axi_io_read/write at link time.  The UART, timer, caches and
 *           interrupt controller stay real (they are modelled by QEMU).
 *
 *           Every device is a register memory: a read returns the last value
 *           written (0 if never written), except for the overrides below,
 *           which give the status values the drivers wait for.  The first
 *           reads of never-written, non-overridden registers are logged, to
 *           show which register needs an override next.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "parameters.h"
#include "sim_sine_lut.h"

/*
 * Set when the IIO UART is opened: from then on the console carries the IIOD
 * protocol, and any sim log line would corrupt it.
 */
static bool sim_quiet;

/* Max "first read of an unknown register" lines printed per device. */
#define SIM_LOG_LIMIT		400

/******************************************************************************/
/* SPI devices                                                                */
/******************************************************************************/

enum sim_spi_dev {
	SIM_SPI_AD9082,		/* PS SPI0 */
	SIM_SPI_HMC7044,	/* PS SPI1 */
	SIM_SPI_NUM
};

/*
 * Status bits the drivers wait for.  On a read, the bits in "set" are forced
 * to 1 and the bits in "clr" to 0; all other bits read back what was last
 * written.  A full-byte value uses set = value, clr = ~value.
 */
struct sim_reg_override {
	uint16_t addr;
	uint8_t set;
	uint8_t clr;
};

#define SIM_VAL(a, v)	{ (a), (v), (uint8_t)~(v) }

/* HMC7044: PLL1 FSM state 2 = locked on CLKIN0; PLL2 locked, no SYSREF sync. */
static const struct sim_reg_override hmc7044_overrides[] = {
	SIM_VAL(0x0082, 0x02),	/* PLL1_STATUS */
	SIM_VAL(0x007D, 0x01),	/* ALARM_READBACK */
};

/* AD9082 Rev. 3 Grade 2 (as on the board). */
static const struct sim_reg_override ad9082_overrides[] = {
	SIM_VAL(0x0003, 0x0F),	/* CHIP_TYPE */
	SIM_VAL(0x0004, 0x82),	/* PROD_ID_LSB */
	SIM_VAL(0x0005, 0x90),	/* PROD_ID_MSB */
	SIM_VAL(0x0006, 0x23),	/* CHIP_GRADE: grade 2, revision 3 */
	SIM_VAL(0x0210, 0xFF),	/* supply monitors: all good */
	SIM_VAL(0x0211, 0xFF),
	SIM_VAL(0x0212, 0xFF),
	SIM_VAL(0x0213, 0xFF),
	SIM_VAL(0x3740, 0x09),	/* boot msg0: boot_done, clk_switch_done */
	SIM_VAL(0x3742, 0xFF),	/* boot msg2: boot complete */
	SIM_VAL(0x2008, 0x03),	/* CLK_PLL_STATUS: DAC PLL locked */
	{ 0x3D26, 0x00, 0x08 },	/* UP_CTRL: spi_edge_interrupt self-clears */
	{ 0x0722, 0x08, 0x00 },	/* PLL_STATUS_LCPLL: JESD (SERDES) PLL locked */
	{ 0x01CA, 0x02, 0x01 },	/* DDSM_FTW_UPDATE: load req self-clears, ack */
	{ 0x01A1, 0x02, 0x01 },	/* DDSC_FTW_UPDATE: same, channel NCO */
	{ 0x00B8, 0x10, 0x02 },	/* SYSREF_MODE: oneshot self-clears, sync done */
	{ 0x21C1, 0x00, 0x89 },	/* rx_set_state1 (rev 2/3): reset, fg cal run and
				 * state-go bits self-clear */
	{ 0x21DD, 0x01, 0x00 },	/* rx_state_status: JRX at idle */
	SIM_VAL(0x21DE, 0x00),	/* rx_scratch0 (rev 2/3): firmware clears 0xAA */
	{ 0x0701, 0x80, 0x00 },	/* PLL_STATUS: JTX PLL locked */
	{ 0x0713, 0x01, 0x00 },	/* PHASE_ESTABLISH_STATUS: JTX phase established */
	{ 0x0711, 0x00, 0x01 },	/* QC_MODE_STATUS: JTX mode valid */
	{ 0x055E, 0x60, 0x10 },	/* JRX_DL_204C_0: state 6 = link good */
};

static struct sim_spi_model {
	const char *name;
	uint8_t mem[0x10000];
	uint8_t written[0x10000 / 8];
	uint8_t reported[0x10000 / 8];
	const struct sim_reg_override *ovr;
	uint32_t n_ovr;
	uint32_t logged;
} sim_spi_models[SIM_SPI_NUM] = {
	[SIM_SPI_AD9082] = {
		.name = "ad9082",
		.ovr = ad9082_overrides,
		.n_ovr = NO_OS_ARRAY_SIZE(ad9082_overrides),
	},
	[SIM_SPI_HMC7044] = {
		.name = "hmc7044",
		.ovr = hmc7044_overrides,
		.n_ovr = NO_OS_ARRAY_SIZE(hmc7044_overrides),
	},
};

static uint8_t sim_spi_reg_read(struct sim_spi_model *m, uint16_t addr)
{
	uint32_t i;

	for (i = 0; i < m->n_ovr; i++)
		if (m->ovr[i].addr == addr)
			return (m->mem[addr] | m->ovr[i].set) & ~m->ovr[i].clr;

	if (!(m->written[addr / 8] & NO_OS_BIT(addr % 8)) &&
	    !(m->reported[addr / 8] & NO_OS_BIT(addr % 8)) &&
	    !sim_quiet && m->logged < SIM_LOG_LIMIT) {
		m->logged++;
		m->reported[addr / 8] |= NO_OS_BIT(addr % 8);
		printf("SIM: %s read of unknown reg 0x%04X -> 0x00\n",
		       m->name, addr);
	}

	return m->mem[addr];
}

static void sim_spi_reg_write(struct sim_spi_model *m, uint16_t addr,
			      uint8_t val)
{
	m->mem[addr] = val;
	m->written[addr / 8] |= NO_OS_BIT(addr % 8);
}

static int32_t sim_spi_init(struct no_os_spi_desc **desc,
			    const struct no_os_spi_init_param *param)
{
	struct no_os_spi_desc *d;

	if (!desc || !param)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->device_id = param->device_id;
	d->chip_select = param->chip_select;
	d->max_speed_hz = param->max_speed_hz;
	d->mode = param->mode;
	*desc = d;

	return 0;
}

/*
 * Both chips use a 16-bit instruction, MSB first: bit 15 = read, then the
 * register address (12 bits on the HMC7044, 15 bits on the AD9082), then one
 * or more data bytes at ascending addresses.
 */
static int32_t sim_spi_write_and_read(struct no_os_spi_desc *desc,
				      uint8_t *data, uint16_t len)
{
	struct sim_spi_model *m;
	uint16_t instr, addr;
	bool rd;
	uint16_t i;

	if (!desc || !data || len < 3)
		return -EINVAL;

	if (desc->device_id == CLK_SPI_DEVICE_ID) {
		m = &sim_spi_models[SIM_SPI_HMC7044];
		instr = ((uint16_t)data[0] << 8) | data[1];
		addr = instr & 0x0FFF;
	} else {
		m = &sim_spi_models[SIM_SPI_AD9082];
		instr = ((uint16_t)data[0] << 8) | data[1];
		addr = instr & 0x7FFF;
	}
	rd = instr & 0x8000;

	for (i = 2; i < len; i++, addr++) {
		if (rd)
			data[i] = sim_spi_reg_read(m, addr);
		else
			sim_spi_reg_write(m, addr, data[i]);
	}

	return 0;
}

static int32_t sim_spi_remove(struct no_os_spi_desc *desc)
{
	no_os_free(desc);

	return 0;
}

const struct no_os_spi_platform_ops sim_spi_ops = {
	.init = sim_spi_init,
	.write_and_read = sim_spi_write_and_read,
	.remove = sim_spi_remove,
};

/******************************************************************************/
/* GPIO                                                                       */
/******************************************************************************/

#define SIM_GPIO_NUM	128

static uint8_t sim_gpio_val[SIM_GPIO_NUM];

static int32_t sim_gpio_get(struct no_os_gpio_desc **desc,
			    const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc *d;

	if (!desc || !param || param->number >= SIM_GPIO_NUM)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->number = param->number;
	d->port = param->port;
	*desc = d;

	return 0;
}

static int32_t sim_gpio_get_optional(struct no_os_gpio_desc **desc,
				     const struct no_os_gpio_init_param *param)
{
	if (!param) {
		*desc = NULL;
		return 0;
	}

	return sim_gpio_get(desc, param);
}

static int32_t sim_gpio_remove(struct no_os_gpio_desc *desc)
{
	no_os_free(desc);

	return 0;
}

static int32_t sim_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	return 0;
}

static int32_t sim_gpio_direction_output(struct no_os_gpio_desc *desc,
		uint8_t value)
{
	sim_gpio_val[desc->number] = value;

	return 0;
}

static int32_t sim_gpio_get_direction(struct no_os_gpio_desc *desc,
				      uint8_t *direction)
{
	*direction = NO_OS_GPIO_OUT;

	return 0;
}

static int32_t sim_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value)
{
	sim_gpio_val[desc->number] = value;

	return 0;
}

/* GT reset-done inputs always report done. */
static int32_t sim_gpio_get_value(struct no_os_gpio_desc *desc, uint8_t *value)
{
	if (desc->number == RX_GT_RESET_DONE || desc->number == TX_GT_RESET_DONE)
		*value = 1;
	else
		*value = sim_gpio_val[desc->number];

	return 0;
}

const struct no_os_gpio_platform_ops sim_gpio_ops = {
	.gpio_ops_get = sim_gpio_get,
	.gpio_ops_get_optional = sim_gpio_get_optional,
	.gpio_ops_remove = sim_gpio_remove,
	.gpio_ops_direction_input = sim_gpio_direction_input,
	.gpio_ops_direction_output = sim_gpio_direction_output,
	.gpio_ops_get_direction = sim_gpio_get_direction,
	.gpio_ops_set_value = sim_gpio_set_value,
	.gpio_ops_get_value = sim_gpio_get_value,
};

/******************************************************************************/
/* UART (IIOD link)                                                           */
/******************************************************************************/

/*
 * Polled PL011 (Versal PS UART0) access.  The real driver receives through
 * the UART interrupt, one byte per armed request.  QEMU's PL011 model raises
 * its RX interrupt only when the FIFO goes from empty to one byte, while the
 * silicon keeps RX / RX-timeout asserted as long as data is waiting.  With
 * back-to-back IIOD commands the FIFO never empties again, so the interrupt
 * driven path stalls in QEMU.  Polling the FIFO avoids that.  The UART is
 * already configured by the BSP for the console.
 */
#define SIM_UART_BASE	XPAR_XUARTPSV_0_BASEADDR
#define SIM_UART_DR	(*(volatile uint32_t *)(uintptr_t)(SIM_UART_BASE + 0x00))
#define SIM_UART_FR	(*(volatile uint32_t *)(uintptr_t)(SIM_UART_BASE + 0x18))
#define SIM_UART_FR_RXFE	NO_OS_BIT(4)
#define SIM_UART_FR_TXFF	NO_OS_BIT(5)

static int32_t sim_uart_init(struct no_os_uart_desc **desc,
			     const struct no_os_uart_init_param *param)
{
	struct no_os_uart_desc *d;

	if (!desc || !param)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->device_id = param->device_id;
	d->baud_rate = param->baud_rate;
	*desc = d;
	sim_quiet = true;

	return 0;
}

static int32_t sim_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			     uint32_t bytes_number)
{
	uint32_t i;

	for (i = 0; i < bytes_number; i++) {
		while (SIM_UART_FR & SIM_UART_FR_RXFE)
			;
		data[i] = SIM_UART_DR & 0xFF;
	}

	return bytes_number;
}

static int32_t sim_uart_read_nonblocking(struct no_os_uart_desc *desc,
		uint8_t *data, uint32_t bytes_number)
{
	uint32_t i = 0;

	while (i < bytes_number && !(SIM_UART_FR & SIM_UART_FR_RXFE))
		data[i++] = SIM_UART_DR & 0xFF;

	return i;
}

static int32_t sim_uart_write(struct no_os_uart_desc *desc,
			      const uint8_t *data, uint32_t bytes_number)
{
	uint32_t i;

	for (i = 0; i < bytes_number; i++) {
		while (SIM_UART_FR & SIM_UART_FR_TXFF)
			;
		SIM_UART_DR = data[i];
	}

	return bytes_number;
}

static uint32_t sim_uart_get_errors(struct no_os_uart_desc *desc)
{
	return 0;
}

static int32_t sim_uart_remove(struct no_os_uart_desc *desc)
{
	no_os_free(desc);

	return 0;
}

const struct no_os_uart_platform_ops sim_uart_ops = {
	.init = sim_uart_init,
	.read = sim_uart_read,
	.write = sim_uart_write,
	.read_nonblocking = sim_uart_read_nonblocking,
	.get_errors = sim_uart_get_errors,
	.remove = sim_uart_remove,
};

/******************************************************************************/
/* AXI (PL) registers                                                         */
/******************************************************************************/

#define SIM_AXI_SLOTS	4096	/* power of two */

struct sim_axi_reg {
	uint32_t addr;
	uint32_t val;
	bool used;
};

static struct sim_axi_reg sim_axi_regs[SIM_AXI_SLOTS];
static uint32_t sim_axi_logged;

/*
 * ADI AXI JESD204 link cores (TX = 204T, RX = 204R), as built for VPK180 M=8:
 * core 1.07.a, 8 lanes, 204C (64b66b), 8-byte link / 12-byte TPL data path
 * (375 MHz link clock -> 250 MHz device clock), link in DATA, SYSREF captured.
 * Measured clocks are the board values (375.003 / 250.002 MHz).
 */
static bool sim_jesd_read(uint32_t base, uint32_t offset, uint32_t *data)
{
	bool tx = (base == TX_JESD_BASEADDR);

	if (base != TX_JESD_BASEADDR && base != RX_JESD_BASEADDR)
		return false;

	switch (offset) {
	case 0x00:			/* VERSION */
		*data = 0x00010761;
		return true;
	case 0x0C:			/* MAGIC */
		*data = tx ? 0x32303454 : 0x32303452;
		return true;
	case 0x10:			/* NUM_LANES */
		*data = 8;
		return true;
	case 0x14:			/* DATA_PATH_WIDTH: TPL 12, link 1 << 3 */
		*data = (12 << 8) | 3;
		return true;
	case 0x18:			/* SYNTH_REG_1: encoder 64b66b */
		*data = 2 << 8;
		return true;
	case 0xC8:			/* LINK_CLK_RATIO: 375.003 MHz */
		*data = 245762;
		return true;
	case 0xCC:			/* DEVICE_CLK_RATIO: 250.002 MHz */
		*data = 163841;
		return true;
	case 0x108:			/* SYSREF_STATUS: captured, no error */
		*data = 0x1;
		return true;
	case 0x280:			/* LINK_STATUS: DATA */
		*data = 0x3;
		return true;
	case 0x300 ... 0x3FF:		/* per-lane (RX): EMB state LOCK */
		*data = ((offset & 0x1F) == 0) ? (4 << 8) : 0;
		return !tx;
	default:
		return false;
	}
}

/*
 * ADI AXI DAC/ADC TPL cores: status OK, interface clock 250.003 MHz x 8 =
 * 2000024414 Hz, as reported on the board.
 */
static bool sim_tpl_read(uint32_t base, uint32_t offset, uint32_t *data)
{
	if (base != TX_CORE_BASEADDR && base != RX_CORE_BASEADDR)
		return false;

	switch (offset) {
	case 0x54:			/* CLK_FREQ (x 100 MHz / 2^16) */
		*data = 163842;
		return true;
	case 0x58:			/* CLK_RATIO */
		*data = 8;
		return true;
	case 0x5C:			/* STATUS: OK */
		*data = 0x1;
		return true;
	default:
		return false;
	}
}

static struct sim_axi_reg *sim_axi_slot(uint32_t addr, bool create)
{
	uint32_t i = (addr >> 2) & (SIM_AXI_SLOTS - 1);
	uint32_t n;

	for (n = 0; n < SIM_AXI_SLOTS; n++, i = (i + 1) & (SIM_AXI_SLOTS - 1)) {
		if (sim_axi_regs[i].used && sim_axi_regs[i].addr == addr)
			return &sim_axi_regs[i];
		if (!sim_axi_regs[i].used) {
			if (!create)
				return NULL;
			sim_axi_regs[i].used = true;
			sim_axi_regs[i].addr = addr;
			return &sim_axi_regs[i];
		}
	}

	return NULL;
}

/*
 * Only the PL (FPGA fabric) register window is modelled.  Everything else,
 * e.g. the DAC sample buffer in DDR that axi_dac_load_custom_data() fills
 * through no_os_axi_io_write(), goes to the real memory access.
 */
#define SIM_PL_START	0xA4000000u
#define SIM_PL_END	0xC0000000u

int32_t __real_no_os_axi_io_read(uint32_t base, uint32_t offset,
				 uint32_t *data);
int32_t __real_no_os_axi_io_write(uint32_t base, uint32_t offset,
				  uint32_t data);

static bool sim_is_pl(uint32_t addr)
{
	return addr >= SIM_PL_START && addr < SIM_PL_END;
}

int32_t __wrap_no_os_axi_io_read(uint32_t base, uint32_t offset,
				 uint32_t *data)
{
	uint32_t addr = base + offset;
	struct sim_axi_reg *r;

	if (!sim_is_pl(addr))
		return __real_no_os_axi_io_read(base, offset, data);

	if (sim_jesd_read(base, offset, data) ||
	    sim_tpl_read(base, offset, data))
		return 0;

	r = sim_axi_slot(addr, false);
	if (r) {
		*data = r->val;
		return 0;
	}

	if (!sim_quiet && sim_axi_logged < SIM_LOG_LIMIT) {
		sim_axi_logged++;
		printf("SIM: axi read of unknown reg 0x%08X (base 0x%08X + 0x%X) -> 0\n",
		       addr, base, offset);
	}
	*data = 0;

	return 0;
}

static uint32_t sim_axi_get(uint32_t addr)
{
	struct sim_axi_reg *r = sim_axi_slot(addr, false);

	return r ? r->val : 0;
}

static int32_t sim_axi_set(uint32_t addr, uint32_t val)
{
	struct sim_axi_reg *r = sim_axi_slot(addr, true);

	if (!r)
		return -ENOMEM;
	r->val = val;

	return 0;
}


/*
 * DAC -> ADC loopback, as on the board's DAC0 -> ADC0 path: the 1 GHz TX main
 * NCO and the 1 GHz RX NCO cancel, and both TPL interfaces run at the same
 * 2 GSPS, so RX channel c sees what TX channel c sends at baseband.  The TX
 * source is read from the emulated DAC TPL registers, so osc / IIO changes
 * show up in the next capture:
 *   DATA_SELECT(c) = 0 (DDS): sum of the channel's two DDS tones
 *     SCALE  @ 0x400 + c*0x40 + t*8: 0x4000 = 1.0, bit 15 = negative
 *     INIT/INCR @ 0x404 + c*0x40 + t*8: phase [31:16], step [15:0], both in
 *     units of 1/65536 of a turn per sample
 *   DATA_SELECT(c) = 2 (DMA): the TX DMA buffer (16-bit samples, the TX
 *     channels interleaved), played cyclically
 *   anything else: 0
 * Time runs on across transfers, so continuous captures stay phase-coherent.
 */
#define SIM_TX_CHANNELS	AD9081_TX_JESD_M

static uint64_t sim_rx_time;		/* samples per channel since start */
static uint32_t sim_tx_buf_addr;	/* last TX DMA source */
static uint32_t sim_tx_buf_bytes;

/* sin() of a 32-bit phase (full turn = 2^32), Q15. */
static int32_t sim_sin(uint32_t phase)
{
	uint32_t idx = phase >> (32 - SIM_SINE_LUT_BITS);	/* 0..1023 */
	uint32_t q = idx >> (SIM_SINE_LUT_BITS - 2);		/* quadrant */
	uint32_t i = idx & ((1u << (SIM_SINE_LUT_BITS - 2)) - 1);

	switch (q) {
	case 0:
		return sim_sine_q[i];
	case 1:
		return sim_sine_q[256 - i];
	case 2:
		return -sim_sine_q[i];
	default:
		return -sim_sine_q[256 - i];
	}
}

static int16_t sim_loopback_sample(uint32_t ch, uint64_t n)
{
	uint32_t cfg = TX_CORE_BASEADDR + 0x400 + ch * 0x40;
	uint32_t sel = sim_axi_get(cfg + 0x18) & 0xF;
	int32_t acc = 0;
	uint32_t t;

	if (sel == 0) {				/* DDS */
		for (t = 0; t < 2; t++) {
			uint32_t sc = sim_axi_get(cfg + t * 8) & 0xFFFF;
			uint32_t ii = sim_axi_get(cfg + 4 + t * 8);
			uint32_t incr = ii & 0xFFFF, init = ii >> 16;
			uint32_t ph = (uint32_t)((init + n * incr) << 16);
			int32_t amp = sc & 0x7FFF;	/* Q14 */

			if (sc & 0x8000)
				amp = -amp;
			acc += (sim_sin(ph) * amp) >> 14;
		}
	} else if (sel == 2 && sim_tx_buf_bytes >= 2) {	/* DMA */
		const int16_t *tx = (const int16_t *)(uintptr_t)sim_tx_buf_addr;
		uint32_t len = sim_tx_buf_bytes / 2;

		acc = tx[(n * SIM_TX_CHANNELS + ch) % len];
	}

	if (acc > 32767)
		acc = 32767;
	if (acc < -32768)
		acc = -32768;

	return (int16_t)acc;
}

/*
 * ADI AXI DMAC.  The RX DMAC is device-to-memory (only DEST is
 * memory-mapped), the TX DMAC memory-to-device (only SRC).  On TRANSFER_SUBMIT
 * the transfer completes at once: an RX transfer fills the destination with
 * the DAC -> ADC loopback samples of the enabled channels (interleaved, 16
 * bit), a TX transfer records its source buffer for the loopback; then
 * IRQ_PENDING reports SOT | EOT.
 */
static bool sim_dmac_write(uint32_t base, uint32_t offset, uint32_t data)
{
	bool rx = (base == RX_DMA_BASEADDR);
	uint32_t i, len, dest;
	int16_t *buf;

	if (base != RX_DMA_BASEADDR && base != TX_DMA_BASEADDR)
		return false;

	switch (offset) {
	case 0x084:			/* IRQ_PENDING: write 1 to clear */
		sim_axi_set(base + offset, sim_axi_get(base + offset) & ~data);
		return true;
	case 0x410:			/* DEST_ADDRESS: memory-mapped on RX only */
		sim_axi_set(base + offset, rx ? data : 0);
		return true;
	case 0x414:			/* SRC_ADDRESS: memory-mapped on TX only */
		sim_axi_set(base + offset, rx ? 0 : data);
		return true;
	case 0x408:			/* TRANSFER_SUBMIT */
		if (!(data & 0x1))
			return true;
		if (rx) {
			uint8_t en[8], n_en = 0, c;

			/*
			 * Like cpack on the board: only the channels enabled in
			 * the RX TPL core (CHAN_CNTRL(c) bit 0) are packed into
			 * the DMA stream, interleaved in channel order.
			 */
			for (c = 0; c < 8; c++)
				if (sim_axi_get(RX_CORE_BASEADDR + 0x400 + c * 0x40) & 0x1)
					en[n_en++] = c;
			if (!n_en)
				for (; n_en < 8; n_en++)
					en[n_en] = n_en;

			dest = sim_axi_get(base + 0x410);
			len = sim_axi_get(base + 0x418) + 1;
			buf = (int16_t *)(uintptr_t)dest;
			for (i = 0; i < len / sizeof(int16_t); i++)
				buf[i] = sim_loopback_sample(en[i % n_en],
							     sim_rx_time + i / n_en);
			sim_rx_time += len / sizeof(int16_t) / n_en;
		} else {
			sim_tx_buf_addr = sim_axi_get(base + 0x414);
			sim_tx_buf_bytes = sim_axi_get(base + 0x418) + 1;
		}
		sim_axi_set(base + 0x408, 0);	/* queue never full */
		sim_axi_set(base + 0x084, 0x3);	/* SOT | EOT */
		return true;
	default:
		return false;
	}
}

int32_t __wrap_no_os_axi_io_write(uint32_t base, uint32_t offset,
				  uint32_t data)
{
	if (!sim_is_pl(base + offset))
		return __real_no_os_axi_io_write(base, offset, data);

	if (sim_dmac_write(base, offset, data))
		return 0;

	return sim_axi_set(base + offset, data);
}
