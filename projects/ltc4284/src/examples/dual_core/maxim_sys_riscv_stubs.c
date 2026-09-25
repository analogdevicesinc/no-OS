/***************************************************************************//**
 *   @file   maxim_sys_riscv_stubs.c
 *   @brief  Minimal MXC_SYS stubs for RISC-V dual-core builds.
 *   @author Analog Devices Inc.
 *
 * RISC-V firmware runs in a minimal environment where:
 * - Clock configuration is handled by ARM core during boot
 * - RISC-V runs at a fixed clock rate (determined at boot)
 * - Peripheral clocks are already enabled by ARM before RISC-V starts
 *
 * These stubs provide no-op or minimal implementations for SDK functions that
 * would normally be in sys_me15.c, which is incompatible with RISC-V due to
 * missing register definitions in the SDK.
 ******************************************************************************/

#include <stdint.h>
#include "mxc_errors.h"
#include "gcr_regs.h"
#include "mxc_sys.h"

/**
 * @brief Return the RISC-V core clock rate (fixed at boot time)
 *
 * The RISC-V clock is configured by the ARM core before booting the
 * coprocessor. For MAX32690, the RISC-V typically runs at the same
 * frequency as the system clock (100 MHz default).
 *
 * @return RISC-V clock frequency in Hz
 */
uint32_t MXC_SYS_RiscVClockRate(void)
{
	/* MAX32690 RISC-V runs at system clock frequency */
	/* Default: 100 MHz (IPO) */
	/* This could be read from GCR registers if needed */
	return 100000000; /* 100 MHz */
}

/**
 * @brief Enable a peripheral clock
 *
 * PCLKDIS0 holds clocks with enum values 0..31; PCLKDIS1 holds 32..63
 * (same encoding convention as the reset registers). Clearing a bit in a
 * PCLKDIS register enables that clock.
 *
 * The ARM core never touches I2C0 in the dual-core example, so the I2C0
 * clock is disabled at reset and must be explicitly enabled here.
 *
 * @param clock Peripheral clock to enable (mxc_sys_periph_clock_t)
 */
void MXC_SYS_ClockEnable(mxc_sys_periph_clock_t clock)
{
	if ((uint32_t)clock < 32) {
		MXC_GCR->pclkdis0 &= ~(1U << (uint32_t)clock);
	} else if ((uint32_t)clock < 64) {
		MXC_GCR->pclkdis1 &= ~(1U << ((uint32_t)clock - 32));
	}
}

/**
 * @brief Disable a peripheral clock (no-op on RISC-V side)
 *
 * Disabling clocks from RISC-V could interfere with ARM-owned peripherals
 * (UART2 console, SEMA). Keep as no-op; the ARM manages its own clocks.
 *
 * @param clock Peripheral clock to disable (mxc_sys_periph_clock_t)
 */
void MXC_SYS_ClockDisable(mxc_sys_periph_clock_t clock)
{
	(void)clock;
}

/**
 * @brief Reset a peripheral (wrapped version with RMW protection)
 *
 * The SDK encodes which GCR reset register to use in the mxc_sys_reset_t value:
 *   RESET0 peripherals: raw bit position 0..31  → GCR->rst0
 *   RESET1 peripherals: bit position + 32       → GCR->rst1 (subtract 32 for mask)
 *
 * The SDK's MXC_SYS_Reset_Periph uses plain writes which clobbers unrelated
 * bits (SMPHR, SIMO) and can corrupt the hardware semaphore. This wrapper
 * uses read-modify-write to preserve other bits.
 *
 * @param reset Peripheral reset bit (mxc_sys_reset_t)
 * @return E_NO_ERROR on success, E_BAD_PARAM if invalid
 */
int __wrap_MXC_SYS_Reset_Periph(mxc_sys_reset_t reset)
{
	uint32_t mask;

	if (reset < 32) {
		/* RESET0 peripheral (e.g. I2C0=16, GPIO0=2, UART0=11) */
		mask = (1U << reset);
		MXC_GCR->rst0 |= mask;
		while (MXC_GCR->rst0 & mask) {}
	} else if (reset < 64) {
		/* RESET1 peripheral (e.g. I2C1, I2C2) — bit pos is (reset - 32) */
		mask = (1U << (reset - 32));
		MXC_GCR->rst1 |= mask;
		while (MXC_GCR->rst1 & mask) {}
	} else {
		return E_BAD_PARAM;
	}

	return E_NO_ERROR;
}
