/***************************************************************************//**
 *   @file   imu_dual_core_example.c
 *   @brief  ARM Cortex-M4F (CPU0) side of the dual-core IMU example.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * The ARM boots the RISC-V core, which polls an ADIS16577-2 IMU with a local
 * bit-banged no-OS SPI backend. The ARM reads an ADXL345 on the same shared pins,
 * integrates the ADIS gyro samples into roll/pitch/yaw angles, and logs both
 * sensors at ~10 Hz.
 *
 * Flow:
 * 1. ARM selects the 100 MHz IPO clock and initializes UART for the console.
 * 2. ARM initializes IPC (doorbells) and the coprocessor (RISC-V CPU1).
 * 3. ARM boots the RISC-V; the RISC-V configures its SPI GPIO pins and detects
 *    the ADIS.
 * 4. ARM waits until the RISC-V reports IMU_DETECTED (adis_init complete), then
 *    initializes the ADXL345 (the RISC-V is in its fast loop and not holding the lock).
 * 5. Loop: wait the RISC-V doorbell, snapshot raw IMU data, integrate it on ARM,
 *    read the ADXL345 under the SPI bus lock, and log.
 *
 * Shared pins: both sensors share MOSI P0.5, MISO P0.6, and SCLK P0.7, with
 * separate GPIO chip-selects. ARM ADXL transactions and RISC-V ADIS transactions
 * are serialized by a hardware-semaphore bus lock (imu_ipc_shared.h).
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "no_os_uart.h"
#include "no_os_coprocessor.h"
#include "no_os_ipc.h"
#include "no_os_delay.h"
#include "maxim_coprocessor.h"
#include "maxim_ipc.h"
#include "maxim_gpio_spi.h"
#include "maxim_sys.h"
#include "adxl345.h"
#include "common_data.h"
#include "imu_ipc_shared.h"
#include "mxc_device.h"
#include "nvic_table.h"

/*
 * The shared __wrap_MXC_SYS_Reset_Periph() lives in maxim_sys.c (enabled via
 * CONFIG_SYS_MAXIM + the -Wl,--wrap link option). It protects the SEMA bus lock
 * from the SDK's destructive peripheral-reset store.
 */

/* Shared cross-core table (fixed SRAM2 pointer; see imu_ipc_shared.h). */
static volatile imu_shared_t *const g_shared =
	(volatile imu_shared_t *)IMU_SHARED_ADDR;

/* Coprocessor (RISC-V CPU1) and IPC (doorbell) descriptors. */
static struct no_os_coprocessor_desc *cpu1_desc;
static struct no_os_ipc_desc *ipc_desc;
static struct no_os_uart_desc *uart_desc;

/*
 * Set by the RISC-V doorbell ISR (see risc_v_doorbell_isr). The RISC-V rings the
 * host doorbell both for normal 100 Hz notifications and, critically, from its
 * trap handler when it faults -- so this flag is the ARM's "the coprocessor
 * signalled something" event, mapped onto the RISCV_IRQn NVIC line.
 */
static volatile uint32_t risc_v_doorbell_count;

/**
 * @brief ARM ISR for the SEMA CM4 doorbell (fired on the RISCV_IRQn NVIC line).
 * The RISC-V rings this on every notification and on a fault. We only latch a
 * counter here and acknowledge the doorbell; the main loop and the fault checker
 * inspect the shared table. Kept minimal (no printf in ISR context).
 */
static void risc_v_doorbell_isr(void)
{
	risc_v_doorbell_count++;
	/* Acknowledge (clear) the CM4 doorbell, keeping it enabled. */
	maxim_ipc_raw_ack_host();
}

/**
 * @brief Print a decoded RISC-V fault report if one was captured, and halt.
 * @return true if a fault was present (and reported); false otherwise.
 */
static bool report_risc_v_fault(void)
{
	if (g_shared->fault_magic != IMU_FAULT_MAGIC &&
	    g_shared->state != IMU_STATE_FAULT)
		return false;

	printf("\r\n[ARM] *** RISC-V FAULT ***\r\n");
	printf("[ARM]   stage=%lu  mcause=0x%08lx  mepc=0x%08lx  mtval=0x%08lx\r\n",
	       (unsigned long)g_shared->stage,
	       (unsigned long)g_shared->fault_cause,
	       (unsigned long)g_shared->fault_epc,
	       (unsigned long)g_shared->fault_tval);
	printf("[ARM]   (stage 1=spi 2=adis_spi 3=reset_cmd 4=reset_wait "
	       "5=selftest 6=diag 7=sync 8=id 9=loop; "
	       "mcause 2=illegal-insn, 5/7=load/store access fault)\r\n");
	return true;
}


/* ADXL345 on the shared SPI pins, chip-select = GPIO P0.19. */
static struct adxl345_dev *adxl_dev;

/*
 * RISC-V boot address. The RISC-V image is embedded into this ARM ELF (in the
 * .riscv_flash section of the dual-core linker script) and runs in place from
 * flash. _riscv_boot is the linker-exported flash entry address.
 */
extern uint8_t _riscv_boot;

/* ------------------------------------------------------------------------- */
/* ADXL345 on the shared bit-banged SPI bus, CS = GPIO P0.19                  */
/* ------------------------------------------------------------------------- */

/*
 * The reusable maxim_gpio_spi driver owns the pin toggling and the single-driver
 * bus-ownership discipline. This core drives only the ADXL CS; SCLK/MOSI are
 * high-Z except while a locked transfer is in flight, so it never fights the
 * RISC-V's ADIS transfers on the shared bus.
 */
static struct max_gpio_spi_init_param arm_spi_extra = {
	.port             = 0,
	.sclk_pin         = IMU_SPI_SCLK_GPIO,
	.mosi_pin         = IMU_SPI_MOSI_GPIO,
	.miso_pin         = IMU_SPI_MISO_GPIO,
	.cs_pin           = IMU_ADXL_CS_GPIO,
	.vddioh           = true,
	.half_period_nops = 20,
};

static struct adxl345_init_param adxl_ip = {
	.communication_type = ADXL345_SPI_COMM,
	.dev_type           = ID_ADXL345,
	.spi_init = {
		.device_id    = 1,
		.max_speed_hz = 500000,
		.chip_select  = 2,
		.mode         = NO_OS_SPI_MODE_3,
		.bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &max_gpio_spi_ops,
		.extra        = &arm_spi_extra,
	},
};

/* ------------------------------------------------------------------------- */
/* Setup helpers                                                             */
/* ------------------------------------------------------------------------- */

static int init_ipc(void)
{
	int ret;
	struct no_os_ipc_init_param param = {
		.id = 0,
		.platform_ops = &max_ipc_ops,
		.extra = NULL
	};

	printf("[ARM] Initializing IPC (doorbells)...\r\n");
	ret = no_os_ipc_init(&ipc_desc, &param);
	if (ret) {
		printf("[ARM] ERROR: IPC init failed (%d)\r\n", ret);
		return ret;
	}
	return 0;
}

static int init_coprocessor(void)
{
	int ret;
	struct max_coprocessor_init_param extra = {
		.boot_addr = (uint32_t)&_riscv_boot,
		.enable_debug = true
	};
	struct no_os_coprocessor_init_param param = {
		.id = 0,
		.platform_ops = &max_coprocessor_ops,
		.extra = &extra
	};

	printf("[ARM] Initializing coprocessor (CPU1)...\r\n");
	ret = no_os_coprocessor_init(&cpu1_desc, &param);
	if (ret) {
		printf("[ARM] ERROR: Coprocessor init failed (%d)\r\n", ret);
		return ret;
	}

	printf("[ARM] Booting coprocessor (CPU1)...\r\n");
	ret = no_os_coprocessor_boot(cpu1_desc);
	if (ret) {
		printf("[ARM] ERROR: Coprocessor boot failed (%d)\r\n", ret);
		return ret;
	}

	/*
	 * Map the RISC-V doorbell to an ARM interrupt. The SEMA CM4 doorbell fires
	 * the RISCV_IRQn NVIC line; the RISC-V rings it on every notification AND
	 * from its trap handler on a fault, so this ISR is our "coprocessor
	 * signalled" event -- including faults we would otherwise never see.
	 */
	MXC_SEMA->irq0 = MXC_F_SEMA_IRQ0_EN;   /* enable CM4 doorbell, clear pending */
	MXC_NVIC_SetVector(RISCV_IRQn, risc_v_doorbell_isr);
	NVIC_EnableIRQ(RISCV_IRQn);
	return 0;
}

/**
 * @brief Initialize the ADXL345, holding the SPI bus lock across every access.
 * @return 0 on success, negative on failure.
 */
static int init_adxl(void)
{
	int ret;

	imu_spi_lock();
	ret = adxl345_init(&adxl_dev, adxl_ip);
	imu_spi_unlock();
	if (ret) {
		printf("[ARM] ERROR: ADXL345 init failed (%d) -- check wiring\r\n",
		       ret);
		return ret;
	}

	/* Put the ADXL345 into measurement mode. */
	imu_spi_lock();
	adxl345_set_power_mode(adxl_dev, 1);
	imu_spi_unlock();

	printf("[ARM] ADXL345 initialized on bit-banged SPI pins\r\n");
	return 0;
}

/* Read the ADXL345 acceleration in milli-g, under the SPI bus lock.
 * adxl345_get_g_xyz() returns float in g; multiply by 1000 for milli-g. */
static void adxl_read_mg(int32_t *mx, int32_t *my, int32_t *mz)
{
	float fx = 0.0f, fy = 0.0f, fz = 0.0f;

	imu_spi_lock();
	adxl345_get_g_xyz(adxl_dev, &fx, &fy, &fz);
	imu_spi_unlock();

	*mx = (int32_t)(fx * 1000.0f);
	*my = (int32_t)(fy * 1000.0f);
	*mz = (int32_t)(fz * 1000.0f);
}

/* ADIS16577-2 gyro raw LSB -> milli-degrees over 2 kHz sample intervals. */
#define ADIS_GYRO_MDEG_DIV  5242880LL

/* ADIS16577-2 accel raw LSB -> milli-g. */
#define ADIS_ACCL_LSB_PER_MG  52429LL

/* Print a signed milli-unit value as a fixed-point "+d.ddd" string. */
static void print_milli(const char *label, int32_t milli)
{
	int32_t whole = milli / 1000;
	int32_t frac = milli % 1000;
	if (frac < 0)
		frac = -frac;
	/* Preserve a leading '-' for values like -0.500. */
	if (milli < 0 && whole == 0)
		printf("%s-0.%03ld", label, (long)frac);
	else
		printf("%s%ld.%03ld", label, (long)whole, (long)frac);
}

/* ------------------------------------------------------------------------- */
/* Main                                                                      */
/* ------------------------------------------------------------------------- */

int imu_dual_core_example_main(void)
{
	int ret;

	/*
	 * Select the 100 MHz IPO before UART init: the RISC-V fetches from flash
	 * and needs this clock, and computing the baud divider against the final
	 * clock avoids corrupting the console.
	 */
	MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
	SystemCoreClockUpdate();

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;
	no_os_uart_stdio(uart_desc);

	printf("\r\n");
	printf("========================================\r\n");
	printf("MAX78000 Dual-Core IMU Example (ARM Side)\r\n");
	printf("========================================\r\n");
	printf("[ARM] CS mapping: ADIS=P0.%lu ADXL=P0.%lu\r\n",
	       (unsigned long)IMU_ADIS_CS_GPIO,
	       (unsigned long)IMU_ADXL_CS_GPIO);
	printf("[ARM] RISC-V firmware embedded in flash at 0x%08lx\r\n",
	       (unsigned long)&_riscv_boot);

	ret = init_ipc();
	if (ret)
		return ret;

	ret = init_coprocessor();
	if (ret)
		return ret;

	/*
	 * Wait for the RISC-V to bring up its SPI GPIO pins before we touch the shared bus, but
	 * with a bounded timeout and fault check so a stuck/faulting coprocessor is
	 * reported instead of hanging the ARM forever. The RISC-V publishes the
	 * shared magic early, advances `state` on success, sets IMU_STATE_ERROR +
	 * init_ret on a handled failure, or trips the trap handler (IMU_STATE_FAULT)
	 * -- and rings the doorbell in every failure case.
	 */
	printf("[ARM] Waiting for RISC-V to configure SPI pins...\r\n");
	{
		uint32_t waited_ms = 0;
		const uint32_t timeout_ms = 2000;

		while (g_shared->magic != IMU_SHARED_MAGIC ||
		       g_shared->state != IMU_STATE_SPI_OK) {
			if (report_risc_v_fault())
				return -EIO;
			if (g_shared->state == IMU_STATE_ERROR) {
				printf("[ARM] ERROR: RISC-V SPI init failed (stage=%lu, "
				       "ret=%ld)\r\n", (unsigned long)g_shared->stage,
				       (long)g_shared->init_ret);
				return -EIO;
			}
			if (waited_ms++ >= timeout_ms) {
				printf("[ARM] ERROR: timed out waiting for RISC-V SPI "
				       "(stage=%lu, state=0x%lx, doorbells=%lu)\r\n",
				       (unsigned long)g_shared->stage,
				       (unsigned long)g_shared->state,
				       (unsigned long)risc_v_doorbell_count);
				return -ETIMEDOUT;
			}
			no_os_mdelay(1);
		}
	}

	/* Wait for the RISC-V to finish reading the ADIS PROD_ID/DIAG. */
	printf("[ARM] Waiting for RISC-V to detect the ADIS16577-2...\r\n");
	{
		uint32_t waited_ms = 0;
		const uint32_t timeout_ms = 2000;

		while (g_shared->state != IMU_STATE_IMU_DETECTED) {
			if (report_risc_v_fault())
				return -EIO;
			if (g_shared->state == IMU_STATE_ERROR) {
				printf("[ARM] ERROR: RISC-V ADIS init failed (stage=%lu, "
				       "ret=%ld)\r\n", (unsigned long)g_shared->stage,
				       (long)g_shared->init_ret);
				return -EIO;
			}
			if (waited_ms++ >= timeout_ms) {
				printf("[ARM] ERROR: timed out waiting for ADIS detect "
				       "(stage=%lu, state=0x%lx)\r\n",
				       (unsigned long)g_shared->stage,
				       (unsigned long)g_shared->state);
				return -ETIMEDOUT;
			}
			no_os_mdelay(1);
		}
	}

	/*
	 * Init the ADXL345 only after IMU_DETECTED: by this point the RISC-V has
	 * completed the slow ADIS software reset + self-test sequence and is in the
	 * fast steady-state polling loop. Holding the bus lock keeps both SPI users
	 * from driving the shared bus at the same time.
	 */
	ret = init_adxl();
	if (ret)
		return ret;
	g_shared->host_ready = 1;

	if (g_shared->prod_id != IMU_ADIS_PROD_ID) {
		printf("[ARM] WARNING: ADIS PROD_ID=%lu (expected %u) -- check "
		       "wiring/CS\r\n", (unsigned long)g_shared->prod_id,
		       IMU_ADIS_PROD_ID);
	} else {
		printf("[ARM] ADIS16577-2 detected: PROD_ID=%lu DIAG=0x%04lx\r\n",
		       (unsigned long)g_shared->prod_id,
		       (unsigned long)g_shared->diag);
	}

	printf("\r\n[ARM] Streaming (ADIS reads on RISC-V, integration on ARM, "
	       "ADXL accel on ARM)...\r\n\r\n");

	/*
	 * Main loop: the RISC-V rings the host doorbell at 100 Hz after updating the
	 * shared table; the doorbell fires risc_v_doorbell_isr on RISCV_IRQn, which
	 * increments risc_v_doorbell_count and acks. We wait for that counter to
	 * advance (WFI to sleep between interrupts), snapshot, read the ADXL, and log
	 * ~10 Hz. A fault from the RISC-V also rings the doorbell, so it is caught
	 * here too.
	 */
	int64_t accum[3] = {0, 0, 0};
	uint64_t total_samples = 0;
	uint32_t prev_dcntr = 0;
	uint32_t last_doorbell = risc_v_doorbell_count;
	uint32_t last_seq = 0;
	uint64_t last_print_samples = 0;
	int have_prev = 0;

	for (;;) {
		int32_t gx, gy, gz, ax, ay, az;
		int32_t roll, pitch, yaw;
		int32_t mgx, mgy, mgz;
		int32_t a_mgx, a_mgy, a_mgz;
		uint32_t seq, dc;
		uint16_t delta;

		/* Sleep until the RISC-V doorbell ISR signals a new event. */
		{
			uint32_t waited_ms = 0;

			while (risc_v_doorbell_count == last_doorbell) {
				if (waited_ms++ >= 1000) {
					printf("[ARM] ERROR: timed out waiting for RISC-V sample "
					       "(stage=%lu, state=0x%lx, seq=%lu, ret=%ld)\r\n",
					       (unsigned long)g_shared->stage,
					       (unsigned long)g_shared->state,
					       (unsigned long)g_shared->seq,
					       (long)g_shared->init_ret);
					return -ETIMEDOUT;
				}
				no_os_mdelay(1);
			}
		}
		last_doorbell = risc_v_doorbell_count;

		/* A fault also rings the doorbell -- report it and stop. */
		if (report_risc_v_fault())
			return -EIO;
		if (g_shared->stage == IMU_STAGE_LOOP_READ_ERROR) {
			printf("[ARM] ERROR: RISC-V ADIS read failed (ret=%ld)\r\n",
			       (long)g_shared->init_ret);
			return -EIO;
		}

		/* Dump trace buffer on first event. */
		if (risc_v_doorbell_count == 1) {
			printf("[ARM] RISC-V trace log:\r\n");
			for (uint32_t i = 0; i < 16; i++) {
				uint32_t event = g_shared->trace_log[i];
				if (event == 0)
					continue;
				printf("[ARM]   [%u] = 0x%04lx\r\n", (unsigned)i,
				       (unsigned long)event);
			}
		}

		/* Snapshot the raw shared IMU payload (ISR already acked the doorbell). */
		seq = g_shared->seq;
		dc = g_shared->data_cntr;
		gx = g_shared->gyro_x;
		gy = g_shared->gyro_y;
		gz = g_shared->gyro_z;
		ax = g_shared->accel_x;
		ay = g_shared->accel_y;
		az = g_shared->accel_z;

		if (!have_prev) {
			prev_dcntr = dc;
			have_prev = 1;
		}

		delta = (uint16_t)((uint16_t)dc - (uint16_t)prev_dcntr);
		prev_dcntr = dc;
		if (delta) {
			accum[0] += (int64_t)gx * delta;
			accum[1] += (int64_t)gy * delta;
			accum[2] += (int64_t)gz * delta;
			total_samples += delta;
		}

		roll = (int32_t)(accum[0] / ADIS_GYRO_MDEG_DIV);
		pitch = (int32_t)(accum[1] / ADIS_GYRO_MDEG_DIV);
		yaw = (int32_t)(accum[2] / ADIS_GYRO_MDEG_DIV);

		/* Read our own sensor (the ADXL345) on the shared bus. */
		adxl_read_mg(&mgx, &mgy, &mgz);

		/*
		 * Decimate console output to ~10 Hz so UART never backpressures. The
		 * RISC-V now free-runs, so gate on the ADIS sample count (which advances
		 * at the fixed 2 kHz ODR) rather than a loop-iteration count: one line
		 * per 200 samples is 10 Hz regardless of how fast we poll. Gyro
		 * integration above still runs on every sample.
		 */
		if (total_samples - last_print_samples < 200)
			continue;
		last_print_samples = total_samples;

		a_mgx = (int32_t)((int64_t)ax / ADIS_ACCL_LSB_PER_MG);
		a_mgy = (int32_t)((int64_t)ay / ADIS_ACCL_LSB_PER_MG);
		a_mgz = (int32_t)((int64_t)az / ADIS_ACCL_LSB_PER_MG);

		print_milli("[IMU] rpy=", roll);
		print_milli(",", pitch);
		print_milli(",", yaw);
		print_milli(" deg  adis_a=", a_mgx);
		print_milli(",", a_mgy);
		print_milli(",", a_mgz);
		print_milli(" g  pmod=", mgx);
		print_milli(",", mgy);
		print_milli(",", mgz);
		/* seq counts RISC-V publishes since the last print; with the ADIS at a
		 * fixed 2 kHz ODR and a 200-sample (10 Hz) print gate, this shows how
		 * many raw samples the coprocessor pushed for this line. */
		printf(" g (n=%lu, seq=%lu, +%lu)\r\n", (unsigned long)total_samples,
		       (unsigned long)seq, (unsigned long)(seq - last_seq));
		last_seq = seq;
	}

	return 0;
}
