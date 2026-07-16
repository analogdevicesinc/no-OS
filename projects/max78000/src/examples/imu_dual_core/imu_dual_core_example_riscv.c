/***************************************************************************//**
 *   @file   imu_dual_core_example_riscv.c
 *   @brief  RISC-V RV32 (CPU1) side of the dual-core IMU example.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Coprocessor firmware that drives an ADIS16577-2 IMU using the real no-OS
 * drivers -- no_os_spi over the Maxim SPI peripheral, plus the adis/adis1657x
 * device driver -- rather than bare-metal register access. It publishes raw
 * gyro/accel/data-counter samples to the ARM core via the SEMA doorbell so the
 * ARM can integrate and log alongside its own ADXL345.
 *
 * The no-OS + Maxim + ADIS sources are cross-compiled for RV32 and linked into
 * this image by maxim_add_riscv_coprocessor(NOOS_DRIVERS ...). The IMU is wired
 * to the MAX78000FTHR SPI header pins, and CPU1 drives them through a local
 * bit-banged no-OS SPI backend.
 *
 * Shared pins: the ADXL345 (ARM side) uses the same SPI pins, so every ADIS
 * transaction is bracketed by the SPI hardware-semaphore bus lock
 * (imu_ipc_shared.h).
 *
 ******************************************************************************
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <stdint.h>

#include "no_os_alloc.h"
#include "no_os_spi.h"
#include "no_os_delay.h"
#include "maxim_gpio_spi.h"
#include "adis.h"
#include "adis1657x.h"

/* SEMA doorbell helpers (maxim_ipc_raw_*) shared with the host side. */
#include "maxim_ipc.h"
/* RISC-V-side runtime helpers (instruction-cache enable). */
#include "maxim_coprocessor_riscv.h"
/* Shared cross-core table + SPI bus-lock helpers. */
#include "imu_ipc_shared.h"

/*
 * The shared __wrap_MXC_SYS_Reset_Periph() lives in maxim_sys.c (compiled into
 * the RISC-V image and enabled via the -Wl,--wrap link option in
 * maxim_coprocessor.cmake). It protects the SEMA bus lock and the SIMO rail from
 * the SDK's destructive peripheral-reset store.
 */

/* Shared table at the fixed SRAM2 address both cores agree on. */
static volatile imu_shared_t *const g_shared =
	(volatile imu_shared_t *)IMU_SHARED_ADDR;

/* ------------------------------------------------------------------------- */
/* Fault reporting                                                           */
/* ------------------------------------------------------------------------- */
/*
 * Record the last risky bring-up step the core entered. If the core then hangs
 * or traps inside that step, the ARM can see exactly where via g_shared->stage
 * even though g_shared->state only advances on success.
 */
static inline void set_stage(uint32_t stage)
{
	g_shared->stage = stage;
}

/*
 * RISC-V trap handler. The stock startup's __isr_vector routes synchronous
 * exceptions (entry 1, illegal instruction) and other traps to weak handlers
 * that just spin forever, which would silently wedge this core. We override the
 * trap entry to capture the trap cause and notify the ARM before halting, so a
 * fault is visible instead of an infinite hang.
 *
 * Implemented as a plain function that reads the RISC-V trap CSRs (mcause/mepc/
 * mtval), publishes them to the shared table with a magic, rings the ARM
 * doorbell, then parks the core. We do not attempt to return from the trap.
 */
__attribute__((used))
void imu_riscv_fault(void)
{
	uint32_t mcause, mepc, mtval;

	__asm__ volatile("csrr %0, mcause" : "=r"(mcause));
	__asm__ volatile("csrr %0, mepc"   : "=r"(mepc));
	__asm__ volatile("csrr %0, mtval"  : "=r"(mtval));

	g_shared->fault_cause = mcause;
	g_shared->fault_epc   = mepc;
	g_shared->fault_tval  = mtval;
	g_shared->fault_magic = IMU_FAULT_MAGIC;
	g_shared->state       = IMU_STATE_FAULT;

	/* Make sure magic is valid so the ARM trusts the table, then notify. */
	if (g_shared->magic != IMU_SHARED_MAGIC)
		g_shared->magic = IMU_SHARED_MAGIC;
	maxim_ipc_raw_ring_host();

	for (;;)
		__asm__ volatile("wfi");
}

/*
 * Override the startup's weak synchronous-exception handlers. On this core the
 * vector table's entry 1 is the illegal-instruction / synchronous-trap path;
 * aliasing these weak symbols to our reporter means a bad instruction, a
 * misaligned/bus access, or an unexpected ecall reports instead of spinning.
 */
void illegal_insn_handler(void) __attribute__((alias("imu_riscv_fault")));
void ecall_insn_handler(void)   __attribute__((alias("imu_riscv_fault")));

/*
 * ADIS16577-2 on the shared bit-banged SPI bus, CS = GPIO P0.11, via the
 * reusable maxim_gpio_spi driver. This core drives only the ADIS CS; SCLK/MOSI
 * are high-Z except during a locked transfer, so it never fights the ARM's ADXL
 * transfers on the shared bus.
 */
static struct max_gpio_spi_init_param rv_spi_extra = {
	.port             = 0,
	.sclk_pin         = IMU_SPI_SCLK_GPIO,
	.mosi_pin         = IMU_SPI_MOSI_GPIO,
	.miso_pin         = IMU_SPI_MISO_GPIO,
	.cs_pin           = IMU_ADIS_CS_GPIO,
	.vddioh           = true,
	.half_period_nops = 20,
};

static struct no_os_spi_init_param adis_spi_ip = {
	.device_id    = 1,
	.max_speed_hz = 500000,
	.chip_select  = 1,
	.mode         = NO_OS_SPI_MODE_3,
	.bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = &max_gpio_spi_ops,
	.extra        = &rv_spi_extra,
};

static struct adis_init_param adis_ip = {
	.info       = &adis1657x_chip_info,
	.spi_init   = &adis_spi_ip,
	.gpio_reset = NULL,               /* no reset pin -> software reset */
	.sync_mode  = ADIS_SYNC_OUTPUT,
	.dev_id     = ADIS16577_2,
};

static int imu_adis_init_staged(struct adis_dev **adis)
{
	struct adis_diag_flags diag_flags;
	struct adis_dev *dev;
	int ret;

	if (!adis_ip.info)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	set_stage(IMU_STAGE_ADIS_SPI);
	ret = no_os_spi_init(&dev->spi_desc, adis_ip.spi_init);
	if (ret)
		goto error_spi;

	dev->current_page = -1;

	dev->dev_id = adis_ip.dev_id;
	dev->info = adis_ip.info;
	dev->int_clk = 2000;
	dev->is_locked = false;
	g_shared->state = IMU_STATE_SPI_OK;

	set_stage(IMU_STAGE_ADIS_RESET_CMD);
	ret = adis_cmd_sw_res(dev);
	if (ret)
		goto error;

	set_stage(IMU_STAGE_ADIS_TEST);
	ret = adis_cmd_snsr_self_test(dev);
	if (ret)
		goto error;

	set_stage(IMU_STAGE_ADIS_DIAG);
	ret = adis_read_diag_stat(dev, &diag_flags);
	if (ret)
		goto error;

	set_stage(IMU_STAGE_ADIS_SYNC);
	ret = adis_write_sync_mode(dev, adis_ip.sync_mode, adis_ip.ext_clk);
	if (ret)
		goto error;

	*adis = dev;
	return 0;

error:
	no_os_spi_remove(dev->spi_desc);
error_spi:
	no_os_free(dev);
	return ret;
}

/**
 * @brief RISC-V entry point (CPU1). Brings up the ADIS via the no-OS driver, then
 * publishes raw samples to the ARM.
 */
__attribute__((used))
int main(void)
{
	struct adis_dev *adis = NULL;
	uint32_t prod_id = 0, diag = 0;
	int ret;
	/* Enable this core's instruction cache before the hot loop. */
	maxim_riscv_icc_enable();

	imu_trace_event(IMU_TRACE_MAIN_ENTER);

	/* Initialize the shared table (invalidate first, publish magic last). */
	g_shared->magic = 0;
	g_shared->trace_idx = 0;
	g_shared->state = IMU_STATE_BOOT;
	g_shared->stage = IMU_STAGE_START;
	g_shared->init_ret = 0;
	g_shared->fault_magic = 0;
	g_shared->fault_cause = 0;
	g_shared->fault_epc = 0;
	g_shared->fault_tval = 0;
	g_shared->seq = 0;
	g_shared->host_ready = 0;
	g_shared->data_cntr = 0;
	g_shared->gyro_x = 0;
	g_shared->gyro_y = 0;
	g_shared->gyro_z = 0;
	g_shared->accel_x = 0;
	g_shared->accel_y = 0;
	g_shared->accel_z = 0;

	/*
	 * Publish the magic early so the ARM can observe the shared table (including
	 * stage / fault fields) throughout bring-up, not only after SPI is up. The
	 * ARM gates on `state`, not just magic, so this does not race the handshake.
	 */
	imu_trace_event(IMU_TRACE_SHARED_INIT);
	g_shared->magic = IMU_SHARED_MAGIC;

	/*
	 * Bring up the ADIS under the bus lock so the ARM cannot use the same SPI
	 * bus while CPU1 runs the reset/self-test sequence.
	 */
	imu_trace_event(IMU_TRACE_ADIS_LOCK);
	imu_spi_lock();
	ret = imu_adis_init_staged(&adis);
	imu_trace_event(IMU_TRACE_ADIS_UNLOCK);
	imu_spi_unlock();
	if (ret) {
		g_shared->init_ret = ret;
		g_shared->state = IMU_STATE_ERROR;
		maxim_ipc_raw_ring_host();
		goto hang;
	}
	/* Publish PROD_ID / DIAG for the ARM (prod_id is the detection truth).
	 * DIAG_STAT is read as a raw 16-bit register (0x02) for a compact word. */
	set_stage(IMU_STAGE_READ_ID);
	imu_spi_lock();
	ret = adis_read_prod_id(adis, &prod_id);
	ret |= adis_read_reg(adis, 0x02, &diag, 2);
	imu_spi_unlock();
	if (ret) {
		g_shared->init_ret = ret;
		g_shared->state = IMU_STATE_ERROR;
		maxim_ipc_raw_ring_host();
		goto hang;
	}
	g_shared->prod_id = prod_id;
	g_shared->diag = diag;
	g_shared->state = IMU_STATE_IMU_DETECTED;
	maxim_ipc_raw_ring_host();

	while (!g_shared->host_ready)
		;

	/* Raw sample loop. */
	set_stage(IMU_STAGE_LOOP);
	g_shared->state = IMU_STATE_RUNNING;
	for (;;) {
		int32_t gx = 0, gy = 0, gz = 0, ax = 0, ay = 0, az = 0;
		uint32_t dc = 0;

		imu_trace_event(IMU_TRACE_LOOP_READ);
		imu_spi_lock();
		ret  = adis_read_x_gyro(adis, &gx);
		ret |= adis_read_y_gyro(adis, &gy);
		ret |= adis_read_z_gyro(adis, &gz);
		ret |= adis_read_x_accl(adis, &ax);
		ret |= adis_read_y_accl(adis, &ay);
		ret |= adis_read_z_accl(adis, &az);
		ret |= adis_read_data_cntr(adis, &dc);
		imu_spi_unlock();

		if (ret) {
			g_shared->init_ret = ret;
			set_stage(IMU_STAGE_LOOP_READ_ERROR);
			maxim_ipc_raw_ring_host();
			/* Back off only on error so a persistent fault does not spam
			 * the doorbell; the success path runs flat-out. */
			no_os_mdelay(10);
			continue;
		}

		/*
		 * Publish only once the previous doorbell has been acked (the host
		 * doorbell is a level, not a counter) so no notification is coalesced
		 * away. Never block on the ARM.
		 */
		if (!maxim_ipc_raw_pending_host()) {
			imu_trace_event(IMU_TRACE_LOOP_PUBLISH);
			g_shared->data_cntr = dc;
			g_shared->gyro_x = gx;
			g_shared->gyro_y = gy;
			g_shared->gyro_z = gz;
			g_shared->accel_x = ax;
			g_shared->accel_y = ay;
			g_shared->accel_z = az;
			g_shared->seq++;

			maxim_ipc_raw_ring_host();
		}

		/*
		 * Pace the loop to ~the ADIS 2 kHz internal ODR (new sample every
		 * ~500 us). Reading faster is useless -- it just re-reads the same
		 * sample (data_cntr delta 0) -- and, critically, holding the shared bus
		 * lock back-to-back with no gap starves the ARM: imu_spi_lock() is an
		 * unfair semaphore spin, so the ARM can only win the bus in the idle
		 * window this delay creates. Without it the ARM never gets to read the
		 * ADXL or print. The delay is OUTSIDE the lock, so the bus is free the
		 * whole time.
		 */
		no_os_udelay(IMU_RV_LOOP_PERIOD_US);
	}

hang:
	/* On fatal error, leave the state visible to the ARM and idle. */
	for (;;)
		__asm__ volatile("wfi");

	return 0;
}
