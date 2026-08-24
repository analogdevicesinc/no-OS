/***************************************************************************//**
 *   @file   main.c
 *   @brief  CAPI coprocessor demo for the MAX78000 EvKit.
********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * Runs on the Cortex-M4 and drives the RV32 core (CPU1) entirely through the
 * CAPI coprocessor API, narrating each step over the EvKit's stdio UART. The
 * point is to show the primitives the API added on top of init/boot/halt:
 * capability negotiation, boot-vector programming with read-back, coprocessor-
 * scoped mode control, and enumerated resource enable/disable.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#include "capi_coprocessor.h"
#include "maxim_capi_coprocessor.h"

#define UART_DEVICE_ID 0
#define UART_IRQ_ID    UART0_IRQn
#define UART_BAUDRATE  115200

/** Arbitrary flash address used only to exercise vector register read-back. */
#define DEMO_VECTOR_ADDR 0x10000040U

static struct max_uart_init_param uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

static struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &uart_extra_ip,
	.platform_ops = &max_uart_ops,
};

/*
 * A valid RV32I program for mode-transition testing: `jal x0, 0` (j .).
 *
 * Keep this in the ARM image's flash rather than reusing the reset vector left
 * by a debugger or bootloader. That old vector can point into stale SRAM; if
 * CPU1 traps there without a valid mtvec, its bus transaction can also stall
 * CPU0 and make the demo appear to hang in capi_coprocessor_set_mode(RUN).
 */
static const uint32_t rv32_idle_loop __attribute__((aligned(4))) = 0x0000006fU;

static const char *mode_name(enum capi_coprocessor_mode mode)
{
	switch (mode) {
	case CAPI_COPROCESSOR_MODE_RUN:
		return "RUN";
	case CAPI_COPROCESSOR_MODE_PARKED:
		return "PARKED";
	case CAPI_COPROCESSOR_MODE_STANDBY:
		return "STANDBY";
	case CAPI_COPROCESSOR_MODE_LOW_POWER:
		return "LOW_POWER";
	case CAPI_COPROCESSOR_MODE_OFF:
		return "OFF";
	default:
		return "?";
	}
}

static const char *resource_name(enum capi_coprocessor_resource_type type)
{
	switch (type) {
	case CAPI_COPROCESSOR_RESOURCE_DATA_LANE:
		return "data lane";
	case CAPI_COPROCESSOR_RESOURCE_CACHE:
		return "cache";
	case CAPI_COPROCESSOR_RESOURCE_ACCELERATOR:
		return "accelerator";
	case CAPI_COPROCESSOR_RESOURCE_POWER_DOMAIN:
		return "power domain";
	default:
		return "?";
	}
}

/** @brief Step 1: decode the capability mask into something readable. */
static int show_boot_caps(struct capi_coprocessor_handle *copro)
{
	static const struct {
		enum capi_coprocessor_boot_cap cap;
		const char *name;
	} caps[] = {
		{ CAPI_COPROCESSOR_BOOT_CAP_VECTOR_BASE, "vector_base" },
		{ CAPI_COPROCESSOR_BOOT_CAP_ENTRY_PC, "entry_pc" },
		{ CAPI_COPROCESSOR_BOOT_CAP_STACK_POINTER, "stack_pointer" },
		{ CAPI_COPROCESSOR_BOOT_CAP_ARGS, "args" },
		{ CAPI_COPROCESSOR_BOOT_CAP_VECTOR_MODE, "vector_mode" },
	};
	uint32_t mask = 0;
	unsigned int i;
	int ret;

	ret = capi_coprocessor_get_boot_caps(copro, &mask);
	if (ret) {
		printf("get_boot_caps failed: %d\n", ret);
		return ret;
	}

	printf("\n-- boot capabilities (mask 0x%08lx)\n", (unsigned long)mask);
	for (i = 0; i < NO_OS_ARRAY_SIZE(caps); i++)
		printf("   %-14s %s\n", caps[i].name,
		       (mask & (uint32_t)caps[i].cap) ? "supported" : "not supported");

	return 0;
}

/** @brief Step 2: program the reset vector and read it back from hardware. */
static int roundtrip_vector(struct capi_coprocessor_handle *copro,
			    uint64_t addr)
{
	uint64_t readback = 0;
	int ret;

	printf("\n-- boot vector round-trip\n");

	ret = capi_coprocessor_set_vector_base(copro, addr);
	if (ret) {
		printf("   set_vector_base(0x%08lx) failed: %d\n",
		       (unsigned long)addr, ret);
		return ret;
	}

	ret = capi_coprocessor_get_vector_base(copro, &readback);
	if (ret) {
		printf("   get_vector_base failed: %d\n", ret);
		return ret;
	}

	printf("   wrote 0x%08lx, read 0x%08lx -> %s\n", (unsigned long)addr,
	       (unsigned long)readback, readback == addr ? "match" : "MISMATCH");

	return readback == addr ? 0 : -EIO;
}

/**
 * @brief Step 3: ask for something the RV32 cannot do.
 *
 * The core has no way to preload a stack pointer, so the driver leaves that bit
 * out of its capability mask and the request is refused instead of being
 * accepted and silently ignored. Getting -ENOTSUP here is the pass condition.
 */
static int show_unsupported_field(struct capi_coprocessor_handle *copro)
{
	struct capi_coprocessor_boot_config config = {0};
	int ret;

	printf("\n-- capability negotiation\n");

	config.vector_base = DEMO_VECTOR_ADDR;
	config.stack_pointer = 0x20003000U;
	config.vector_mode = CAPI_COPROCESSOR_VECTOR_ENTRY;

	ret = capi_coprocessor_set_boot_config(copro, &config);
	printf("   set_boot_config with stack_pointer -> %d (%s)\n", ret,
	       ret == -ENOTSUP ? "-ENOTSUP, as expected" : "unexpected");

	return ret == -ENOTSUP ? 0 : -EIO;
}

/** @brief Step 4: walk the mode machine, reading back after each transition. */
static int walk_modes(struct capi_coprocessor_handle *copro)
{
	static const enum capi_coprocessor_mode sequence[] = {
		CAPI_COPROCESSOR_MODE_RUN,
		CAPI_COPROCESSOR_MODE_PARKED,
		CAPI_COPROCESSOR_MODE_STANDBY,
		CAPI_COPROCESSOR_MODE_LOW_POWER,
		CAPI_COPROCESSOR_MODE_RUN,
		CAPI_COPROCESSOR_MODE_OFF,
	};
	enum capi_coprocessor_mode mode;
	unsigned int i;
	int ret;

	printf("\n-- mode walk (coprocessor-scoped; the host keeps running)\n");

	for (i = 0; i < NO_OS_ARRAY_SIZE(sequence); i++) {
		ret = capi_coprocessor_set_mode(copro, sequence[i]);
		if (ret) {
			printf("   set_mode(%s) failed: %d\n",
			       mode_name(sequence[i]), ret);
			return ret;
		}

		/* Let the gate settle before trusting the read-back. */
		no_os_mdelay(1);

		ret = capi_coprocessor_get_mode(copro, &mode);
		if (ret) {
			printf("   get_mode failed: %d\n", ret);
			return ret;
		}

		printf("   requested %-10s read %-10s %s\n",
		       mode_name(sequence[i]), mode_name(mode),
		       mode == sequence[i] ? "" : "(differs)");
	}

	return 0;
}

/** @brief Step 5: enumerate every resource class, then toggle each member. */
static int toggle_resources(struct capi_coprocessor_handle *copro)
{
	static const enum capi_coprocessor_resource_type types[] = {
		CAPI_COPROCESSOR_RESOURCE_DATA_LANE,
		CAPI_COPROCESSOR_RESOURCE_CACHE,
		CAPI_COPROCESSOR_RESOURCE_ACCELERATOR,
		CAPI_COPROCESSOR_RESOURCE_POWER_DOMAIN,
	};
	unsigned int i;
	uint32_t index;
	uint32_t count;
	bool enabled;
	int ret;

	printf("\n-- resources\n");

	for (i = 0; i < NO_OS_ARRAY_SIZE(types); i++) {
		ret = capi_coprocessor_resource_get_count(copro, types[i], &count);
		if (ret) {
			printf("   %s: get_count failed: %d\n",
			       resource_name(types[i]), ret);
			return ret;
		}

		printf("   %-13s count %lu\n", resource_name(types[i]),
		       (unsigned long)count);

		for (index = 0; index < count; index++) {
			ret = capi_coprocessor_resource_get_enabled(copro, types[i],
					index, &enabled);
			if (ret)
				return ret;
			printf("      [%lu] initially %s\n", (unsigned long)index,
			       enabled ? "enabled" : "disabled");

			/* Off, then back on, reporting the state each time. */
			ret = capi_coprocessor_resource_set_enabled(copro, types[i],
					index, false);
			if (ret)
				return ret;
			ret = capi_coprocessor_resource_get_enabled(copro, types[i],
					index, &enabled);
			if (ret)
				return ret;
			printf("      [%lu] after disable -> %s\n",
			       (unsigned long)index, enabled ? "enabled" : "disabled");

			ret = capi_coprocessor_resource_set_enabled(copro, types[i],
					index, true);
			if (ret)
				return ret;
			ret = capi_coprocessor_resource_get_enabled(copro, types[i],
					index, &enabled);
			if (ret)
				return ret;
			printf("      [%lu] after enable  -> %s\n",
			       (unsigned long)index, enabled ? "enabled" : "disabled");
		}

		/* One past the end must be refused, not clamped. */
		ret = capi_coprocessor_resource_set_enabled(copro, types[i], count, true);
		printf("      index %lu (out of range) -> %d%s\n",
		       (unsigned long)count, ret,
		       ret == -EINVAL ? " (-EINVAL, as expected)" : "");
	}

	return 0;
}

int main(void)
{
	const struct capi_coprocessor_config copro_ip = {
		.identifier = MAXIM_CAPI_COPROCESSOR_RV32,
		.extra = NULL,
		.ops = &maxim_capi_coprocessor_ops,
	};
	struct capi_coprocessor_handle *copro = NULL;
	struct no_os_uart_desc *uart;
	int ret;

	ret = no_os_uart_init(&uart, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	printf("\nCAPI coprocessor demo - MAX78000 EvKit (RV32 / CPU1)\n");

	ret = capi_coprocessor_init(&copro, &copro_ip);
	if (ret) {
		printf("capi_coprocessor_init failed: %d\n", ret);
		return ret;
	}

	ret = show_boot_caps(copro);
	if (ret)
		goto out;

	ret = roundtrip_vector(copro, DEMO_VECTOR_ADDR);
	if (ret)
		goto out;

	ret = show_unsupported_field(copro);
	if (ret)
		goto out;

	/* CPU1 is released during the mode walk, so give it known-valid code. */
	ret = roundtrip_vector(copro, (uint64_t)(uintptr_t)&rv32_idle_loop);
	if (ret)
		goto out;

	ret = walk_modes(copro);
	if (ret)
		goto out;

	ret = toggle_resources(copro);
	if (ret)
		goto out;

	printf("\ndone\n");

out:
	capi_coprocessor_deinit(copro);

	return ret;
}
