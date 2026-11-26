/***************************************************************************//**
 *   @file   sim_platform.c
 *   @brief  Software-in-the-loop stand-ins for the AD9084-EBZ / Agilex 5
 *           hardware, for a host (x86, 32-bit) simulation build.
 *
 *           Built only with -DAD9088_SIM=ON (preset native-i386).  The project
 *           keeps using altera_spi_ops / altera_gpio_ops; CMake renames them
 *           to sim_spi_ops / sim_gpio_ops for the ad9088 target and wraps
 *           no_os_axi_io_read/write at link time.  The few FPGA registers the
 *           Agilex code reaches through raw pointers (GTS PLL and transceiver
 *           windows) are backed by plain memory mapped at their addresses.
 *
 *           The AD9084 is handled by apollo_model.c.  The clock chips and
 *           the FPGA cores are register memories: a read returns the last
 *           value written, or a fixed value for ID and status registers.
 *           The first SPI frames per device and the first reads of
 *           never-written FPGA registers are logged.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "parameters.h"
#include "apollo_model.h"

/* Max "first read of an unknown register" lines printed per device. */
#define SIM_LOG_LIMIT		200

/******************************************************************************/
/* Raw-pointer FPGA windows                                                   */
/******************************************************************************/

/*
 * altera_gts_xcvr.h pokes the GTS PLL refclk byte and the link-management
 * RESETN / STATUS registers through raw pointers.  Back those pages with
 * memory at their real addresses (possible in a 32-bit process) and report
 * the transceivers ready (STATUS bit 0).
 */
static const uint32_t sim_raw_pages[] = {
	GTS_PLL_RX_BASEADDR, GTS_PLL_TX_BASEADDR,
	RX_XCVR_BASEADDR, TX_XCVR_BASEADDR,
};

__attribute__((constructor))
static void sim_map_raw_windows(void)
{
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(sim_raw_pages); i++) {
		uintptr_t page = sim_raw_pages[i] & ~0xFFFu;
		void *p = mmap((void *)page, 0x1000, PROT_READ | PROT_WRITE,
			       MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
			       -1, 0);

		if (p != (void *)page) {
			fprintf(stderr, "SIM: cannot map 0x%08x\n",
				(unsigned)page);
			exit(1);
		}
	}

	*(volatile uint32_t *)(uintptr_t)(RX_XCVR_BASEADDR + 0x14) = 0x1;
	*(volatile uint32_t *)(uintptr_t)(TX_XCVR_BASEADDR + 0x14) = 0x1;
}

/******************************************************************************/
/* SPI devices                                                                */
/******************************************************************************/

enum sim_spi_dev {
	SIM_SPI_APOLLO,
	SIM_SPI_ADF4382,
	SIM_SPI_HMC7044,
	SIM_SPI_ADF4030,
	SIM_SPI_UNKNOWN,
	SIM_SPI_NUM
};

static const char *const sim_spi_names[SIM_SPI_NUM] = {
	"apollo", "adf4382", "hmc7044", "adf4030", "spi?"
};

struct sim_spi_dev_state {
	uint32_t transfers;
	uint32_t logged;
};

static struct sim_spi_dev_state sim_spi_state[SIM_SPI_NUM];

struct sim_spi_priv {
	enum sim_spi_dev dev;
};

static enum sim_spi_dev sim_spi_identify(const struct no_os_spi_init_param *p)
{
	if (p->extra == &apollo_spi_extra_ip)
		return SIM_SPI_APOLLO;

	if (p->extra == &clk_spi_extra_ip) {
		switch (p->chip_select) {
		case SPI_CS_ADF4382:
			return SIM_SPI_ADF4382;
		case SPI_CS_HMC7044:
			return SIM_SPI_HMC7044;
		case SPI_CS_ADF4030:
			return SIM_SPI_ADF4030;
		default:
			break;
		}
	}

	return SIM_SPI_UNKNOWN;
}

static int32_t sim_spi_init(struct no_os_spi_desc **desc,
			    const struct no_os_spi_init_param *param)
{
	struct no_os_spi_desc *d;
	struct sim_spi_priv *priv;

	if (!desc || !param)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	priv = no_os_calloc(1, sizeof(*priv));
	if (!d || !priv) {
		no_os_free(d);
		no_os_free(priv);
		return -ENOMEM;
	}

	priv->dev = sim_spi_identify(param);
	d->device_id = param->device_id;
	d->chip_select = param->chip_select;
	d->max_speed_hz = param->max_speed_hz;
	d->mode = param->mode;
	d->extra = priv;
	*desc = d;

	return 0;
}

/* 15-bit register address space per device. */
#define SIM_SPI_REGS	0x8000

static uint8_t sim_spi_regs[SIM_SPI_NUM][SIM_SPI_REGS];

/* Read-only registers (IDs, status): always read back this value. */
struct sim_spi_ro {
	enum sim_spi_dev dev;
	uint16_t addr;
	uint8_t val;
};

static const struct sim_spi_ro sim_spi_ro_regs[] = {
	{ SIM_SPI_ADF4382, 0x67, 11 },	/* chip version U5_A */
	{ SIM_SPI_ADF4382, 0x58, 0x09 },	/* LOCKED, REF OK, FSM idle */
};

static bool sim_spi_ro_get(enum sim_spi_dev dev, uint16_t addr, uint8_t *val)
{
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(sim_spi_ro_regs); i++) {
		if (sim_spi_ro_regs[i].dev == dev &&
		    sim_spi_ro_regs[i].addr == addr) {
			*val = sim_spi_ro_regs[i].val;
			return true;
		}
	}

	return false;
}

/*
 * Every device is a register memory.  Frame: two address bytes (read flag
 * 0x80 in the first, 15-bit address), then data bytes streamed at
 * incrementing addresses.  Reads return the last value written.
 */
static int32_t sim_spi_write_and_read(struct no_os_spi_desc *desc,
				      uint8_t *data, uint16_t len)
{
	struct sim_spi_priv *priv;
	struct sim_spi_dev_state *st;
	uint8_t *regs;
	uint16_t addr;
	bool rd;
	uint16_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	priv = desc->extra;
	st = &sim_spi_state[priv->dev];
	regs = sim_spi_regs[priv->dev];
	st->transfers++;

	if (st->logged < SIM_LOG_LIMIT) {
		st->logged++;
		printf("SIM: %s spi tx", sim_spi_names[priv->dev]);
		for (i = 0; i < len && i < 8; i++)
			printf(" %02x", data[i]);
		printf("%s\n", len > 8 ? " ..." : "");
	}

	if (priv->dev == SIM_SPI_APOLLO) {
		apollo_model_xfer(data, len);
		return 0;
	}

	if (len < 3)
		return 0;

	rd = data[0] & 0x80;
	addr = ((data[0] & 0x7F) << 8) | data[1];

	for (i = 2; i < len; i++, addr = (addr + 1) & (SIM_SPI_REGS - 1)) {
		if (rd) {
			if (!sim_spi_ro_get(priv->dev, addr, &data[i]))
				data[i] = regs[addr];
		} else
			regs[addr] = data[i];
	}

	return 0;
}

static int32_t sim_spi_remove(struct no_os_spi_desc *desc)
{
	if (desc)
		no_os_free(desc->extra);
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

static int32_t sim_gpio_get_value(struct no_os_gpio_desc *desc, uint8_t *value)
{
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
/* AXI (FPGA) registers                                                       */
/******************************************************************************/

#define SIM_AXI_SLOTS	4096	/* power of two */

struct sim_axi_reg {
	uint32_t addr;
	uint32_t val;
	bool used;
};

static struct sim_axi_reg sim_axi_regs[SIM_AXI_SLOTS];
static uint32_t sim_axi_logged;

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

int32_t __wrap_no_os_axi_io_read(uint32_t base, uint32_t offset,
				 uint32_t *data)
{
	struct sim_axi_reg *r = sim_axi_slot(base + offset, false);

	if (r) {
		*data = r->val;
		return 0;
	}

	if (sim_axi_logged < SIM_LOG_LIMIT) {
		sim_axi_logged++;
		printf("SIM: axi read of unknown reg 0x%08X (base 0x%08X + 0x%X) -> 0\n",
		       (unsigned)(base + offset), (unsigned)base,
		       (unsigned)offset);
	}
	*data = 0;

	return 0;
}

int32_t __wrap_no_os_axi_io_write(uint32_t base, uint32_t offset,
				  uint32_t data)
{
	struct sim_axi_reg *r = sim_axi_slot(base + offset, true);

	if (!r)
		return -ENOMEM;
	r->val = data;

	return 0;
}

/*
 * Power-on values of the FPGA core registers the drivers read but never
 * write (IDs, versions, synthesis parameters).  Loaded before main().
 */
struct sim_axi_seed {
	uint32_t addr;
	uint32_t val;
};

#define SIM_JESD_MAGIC(c)	(('2' << 24) | ('0' << 16) | ('4' << 8) | (c))

static const struct sim_axi_seed sim_axi_seeds[] = {
	/* JESD204 link layer: v1.4.a, 2 lanes, 4-byte path, 8b10b */
	{ RX_JESD_BASEADDR + 0x00, 0x00010461 },
	{ RX_JESD_BASEADDR + 0x0C, SIM_JESD_MAGIC('R') },
	{ RX_JESD_BASEADDR + 0x10, 2 },
	{ RX_JESD_BASEADDR + 0x14, (4 << 8) | 2 },
	{ RX_JESD_BASEADDR + 0x18, 1 << 8 },
	{ TX_JESD_BASEADDR + 0x00, 0x00010461 },
	{ TX_JESD_BASEADDR + 0x0C, SIM_JESD_MAGIC('T') },
	{ TX_JESD_BASEADDR + 0x10, 2 },
	{ TX_JESD_BASEADDR + 0x14, (4 << 8) | 2 },
	{ TX_JESD_BASEADDR + 0x18, 1 << 8 },
};

__attribute__((constructor))
static void sim_axi_load_seeds(void)
{
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(sim_axi_seeds); i++)
		__wrap_no_os_axi_io_write(sim_axi_seeds[i].addr, 0,
					  sim_axi_seeds[i].val);
}
